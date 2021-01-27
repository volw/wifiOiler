// ACHTUNG: defines in wifioiler.h beachten!!
#include <ESP8266WiFiMulti.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <TinyGPS++.h>
#include "src/biLedx/biLedx.h"  // lib für nicht-modales Blinken (zweifarbiger) LEDs
#include "wifiOiler.h"
#include "configuration.h"
#ifdef _SPIFFS_
  #include "src/SPIFFSlogger.h"
  SPIFFSlogger myLogger;  // Serial und/oder in Datei
#else
  #include "src/LittleFSlogger.h"
  LittleFSlogger myLogger;  // Serial und/oder in Datei
#endif
#include "oilerDisplay.h"

DNSServer dnsServer;
ESP8266WebServer webServer(80);
ESP8266WiFiMulti wifiMulti;
File inFile, outFile, fsUploadFile;
Configuration conf;

//********** GPS ***********:
char     gpsBuffer[GPS_BUFFER_LENGTH] = ""; // Puffer für empfangene GPS-Daten
uint8_t  gpsBufferIndex = 0;  // Index für GPS Buffer
uint32_t oldLocationAge = 0;  // Zeitpunkt (millis) des letzten Fixpunktes
float    oldLat = 0;          // letzte gültige Koordinate (millis, Latitude = Breitengrad)
float    oldLng = 0;          // letzte gültige Koordinate (millis, Longitude = Längengrad)
uint32_t lastGPSData = 0;     // Zeitpunkt (millis) der letzten GPS-Auswertung (egal was, Hauptsache gültiger Satz)
uint32_t lastGPScheck = 0;    //20200309,I: wird benötigt, damit finaler Check in checkGPSdata() nur 1/s aufgerufen wird

//********** Check Movement ****:
uint32_t lastMovementCheck = 0;     // pro Sekunde nur ein Check
uint8_t  movementCounter   = 0;     // Anzahl Movements (bei Bewegung ++ (max 30), bei Stillstand -- (min 0))
double   lastTotalDist     = 0;     // um real Movement zu checken
bool     lastMovingState   = false; // only for Debug
uint8_t  oilCounter        = 0;     // Dauerpumpen nach x mal aus (wenn Kiste fährt)

//********** WiFi sleep ****:
uint32_t lastWiFiCheck = 0;    // Zähler für Movement Detection
uint32_t lastMoveMillis = 0;   // Zeitpunkt der letzten "echten" Bewegung (des Bikes, gefiltert (GPS), ohne Simulation, wird in analyzeGPSInfo() gesetzt)
bool     wifiSleeping = true;  // Kennzeichen, ob WiFi ausgeschaltet ist oder nicht (nach Start erst mal aus)
uint32_t switchOnMillis = 0;   // wann WiFi aktiviert (danach wird 30s nicht automatisch ausgeschaltet)

//**** GPS Track Recording ***:
String   gpsTrackFilename = "";// Dateiname (wird gebildet aus Datum und Uhrzeit (vom Satelliten))
bool     validGPSFilename = false; // ein gültiger Dateiname (mit Datum <> 0) wurde gebildet... (falls Datum == 0 bleiben sollten, wird nix geschrieben)
uint16_t totalGPSwrite    = 0; // Anzahl geschriebener Fixpunkte seit Start
uint16_t totalWriteErrors = 0; // Anzahl fehlgeschlagener Schreibvorgänge (nicht geschriebene Fixpunkte) - (bis jetzt eigentlich immer 0)
uint8_t  currentfpw       = 0; // akuelle Anzahl Sekunden zwischen zu schreibenden Fixpunkten (der File system check kann das temp. auf 0 setzen = deaktiviert, falls zu wenig Platz)
uint32_t lastGPSwrite     = 0; // Zeitpunkt des letzte Schreibvorgangs, wird verwendet, um nur alle x Sekunden einen Fixpunkt zu schreiben (s. Configuration)
uint32_t lastFScheck      = 0; // Zeitpunkt des letzten FS check (auf freien Speicherplatz, s. checkFilesystemSpace())
float    cumDist          = 0; // aufsummierte Distanz zwischen den Fixpunkten (bis zum Schreiben)
double   totalDist        = 0; // gesamt gefahrene Strecke lt. GPS (ohne Simulation)
TinyGPSPlus gpsNew;            // GPS-Modul

//********** Pumpe **********:
float     meterSincePump = 0;     // Meter seit letztem Pumpen, kann nicht überlaufen, selbst wenn Pumpen deaktiviert... ;-)
uint16_t  totalPumpCount = 0;     // Summe aller Pumpaktionen seit Start
tPumpMode pumpMode = MODE_NORMAL; // aktueller Modus der Pumpe (Normal, Regen, Offroad, ...)

//********** other **********:
bool     maintenanceMode = false;   // Wartungsmodus -> keine GPS Behandlung und kein Notfallprogramm, Pumpen nur, wenn Dauerpumpen eingestellt wird.
uint32_t showMaintMode = 0;         // Zeitpunkt (millis) des letzten LED Signals (dass Wartungsmodus aktiv ist)

//**** Button **************:
uint32_t lastPressed = 0;
uint32_t millisPressed = 0;
uint8_t  buttonState = LOW;
uint8_t  lastButtonState = LOW;
 
biLedx      myLedx(LED_GRUEN, LED_ROT, LOW);  // LOW = Anschaltsignal
biLedx      myPumpx(PUMP_PIN);                // default HIGH = Anschaltsignal

oilerDisplay myDisplay;

#include "button.h"
#include "configureGPS.h"
#include "GPS.h"
#include "FSBrowser.h"
#include "oiler.h"
#include "update.h"
#include "upload.h"
#include "WiFi.h"
#include "webHandler.h"

/*******************************************************************
 ******************** Setup ****************************************
 *******************************************************************/
void setup() 
{    
  myLedx.on(LED_GRUEN); // LED an waehrend Initialisierung
  Serial.begin(9600);

  myDisplay.Init();
  myDisplay.PrintMessage("Init", 10000);

  // schon mal was auf's Display bringen (wird erst in Loop angezeigt über Check()):
  myDisplay.PrintModeStr(getPumpModeStr(pumpMode));
  myDisplay.PrintMeter(getModeMeters(pumpMode));
  myDisplay.PrintKmh(0);

  if (!_FILESYS.begin()) {
    Serial.println(F("Fehler bei Initialisierung des Dateisystems!"));
  }

  conf.read();
  myDisplay.MessageAdd(".", 10000);
  currentfpw = conf.fpw;  // merken, currentfpw bestimmt je nach Platz im FS, ob Tracks geschrieben werden...

  // Logging in Datei und Serial (sonst als 2. Parameter: false)
  // VORSICHT: falls Datei sehr groß wird (250KB reichen schon fast), können Schreiboperationen (incl.
  // Öffnen und schließen der Datei seeehr langsam sein und das ganze Ding voll ausbremsen - also nur kurz
  // aktiviert lassen (ein paar Stunden oder sogar 1-2 Tage geht aber schon)!
  if (!myLogger.begin(conf.lgf ? "/myLogger.txt" : "", conf.lgs)) {
    Serial.println(F("Fehler bei Initialisierung des (LittleFS) Logger!"));
  }
  myDisplay.MessageAdd(".", 10000);
  
  DEBUG_OUT.println(F("\n********** Starting..."));

  setupButton();

  //TODO: +++hier könnte noch ein Check auf Track-Dateien < x bytes erfolgen - die dann löschen

  checkforUpdate();
  myDisplay.MessageAdd(".", 10000);

  checkFilesystemSpace(); // tracking deaktivieren, wenn zu wenig Platz
  myDisplay.MessageAdd(".", 10000);

  // WiFi wird beim Starten ausgeschaltet und kann bei Bedarf (Button) angeschaltet werden:
  WiFi.forceSleepBegin(); // Wifi off
  wifiSleeping = true;

  setupGPS(); // Abfrage auf Wartungsmodus innerhalb der Funktion
  myDisplay.MessageAdd(".", 10000);

  setupWebServer();
  delay(100);
  myDisplay.MessageAdd(".", 10000);

  checkTank();  // Öltank leer?

  DEBUG_OUT.print(F("This is wifiOiler version "));
  DEBUG_OUT.print(VERSION);
  DEBUG_OUT.print(F(" on board "));
  DEBUG_OUT.println(BOARD_TYPE);

  if (conf.wso || maintenanceMode) 
    toggleWiFi();
  else 
    myLedx.off();
  myDisplay.PrintMessage("wifiOiler\n  v");
  myDisplay.MessageAdd(VERSION,2000);
  #ifdef _NO_PUMP_
    DEBUG_OUT.println(F("ACHTUNG: Pumpe dauerhaft deaktiviert!!"));
    myDisplay.PrintAckMessage("Pumpe\ndeaktiv.\n(confirm)");
  #endif
  DEBUG_OUT.println(F("-------- Initialisierung beendet."));
}

/*******************************************************************
 ***************** Main Loop ***************************************
 *******************************************************************/
void loop() {
  if (!wifiSleeping) {
    MDNS.update();
    dnsServer.processNextRequest();
    webServer.handleClient();
    checkWiFi();
  }
  myLedx.checkState();  // LED-Behandlung
  myPumpx.checkState(); // Behandlung Pumpe
  checkButton();        // Button Abfrage  
  checkGPSdata();       // GPS Daten lesen, falls welche da sind
  checkMovement();      // Überprüfung auf Bewegung
  myDisplay.Check();    // Display handling
  
  if (maintenanceMode && (millis() > (showMaintMode + 10000UL))) {
    // noch ne kleine Spezialroutine, um Wartungsmodus anzuzeigen:
    myLedx.start LED_SHOW_MAINTENANCE;
    showMaintMode = millis();
  }
  if (updateResult == UPD_REBOOT_NOW) {
    checkforUpdate();
    updateResult = UPD_NOT_ACTIVE; // falls er wieder zurückkommt...
  }
}
