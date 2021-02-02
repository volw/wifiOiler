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

DNSServer GVdnsServer;
ESP8266WebServer GVwebServer(80);
ESP8266WiFiMulti GVwifiMulti;
File GVoutFile, GVfsUploadFile;
Configuration GVoilerConf;

// alle globalen Variablen fangen mit GV.. an:
//********** GPS ***********:
char     GVgpsBuffer[GPS_BUFFER_LENGTH] = ""; // Puffer für empfangene GPS-Daten
uint8_t  GVgpsBufferIndex = 0;  // Index für GPS Buffer
uint32_t GVoldLocationAge = 0;  // Zeitpunkt (millis) des letzten Fixpunktes
float    GVoldLat = 0;          // letzte gültige Koordinate (millis, Latitude = Breitengrad)
float    GVoldLng = 0;          // letzte gültige Koordinate (millis, Longitude = Längengrad)
uint32_t GVlastGPSData = 0;     // Zeitpunkt (millis) der letzten GPS-Auswertung (egal was, Hauptsache gültiger Satz)
uint32_t GVlastGPScheck = 0;    // wird benötigt, damit finaler Check in checkGPSdata() nur 1/s aufgerufen wird

//********** Check Movement ****:
uint32_t GVlastMovementCheck = 0;     // pro Sekunde nur ein Check
uint8_t  GVmovementCounter   = 0;     // Anzahl Movements (bei Bewegung ++ (max 30), bei Stillstand -- (min 0))
double   GVlastTotalDist     = 0;     // um real Movement zu checken
bool     GVlastMovingState   = false; // only for Debug
uint8_t  GVoilCounter        = 0;     // Dauerpumpen nach x mal aus (wenn Kiste fährt)

//********** WiFi sleep ****:
uint32_t GVlastWiFiCheck = 0;    // Zähler für Movement Detection
bool     GVwifiSleeping = true;  // Kennzeichen, ob WiFi ausgeschaltet ist oder nicht (nach Start erst mal aus)
uint32_t GVswitchOnMillis = 0;   // wann WiFi aktiviert (danach wird 30s nicht automatisch ausgeschaltet)

//**** GPS Track Recording ***:
String   GVgpsTrackFilename = "";// Dateiname (wird gebildet aus Datum und Uhrzeit (vom Satelliten))
bool     GVvalidGPSFilename = false; // ein gültiger Dateiname (mit Datum <> 0) wurde gebildet... (falls Datum == 0 bleiben sollten, wird nix geschrieben)
uint16_t GVtotalGPSwrite    = 0; // Anzahl geschriebener Fixpunkte seit Start
uint16_t GVtotalWriteErrors = 0; // Anzahl fehlgeschlagener Schreibvorgänge (nicht geschriebene Fixpunkte) - (bis jetzt eigentlich immer 0)
uint8_t  GVcurrentfpw       = 0; // akuelle Anzahl Sekunden zwischen zu schreibenden Fixpunkten (der File system check kann das temp. auf 0 setzen = deaktiviert, falls zu wenig Platz)
uint32_t GVlastGPSwrite     = 0; // Zeitpunkt des letzte Schreibvorgangs, wird verwendet, um nur alle x Sekunden einen Fixpunkt zu schreiben (s. Configuration)
uint32_t GVlastFScheck      = 0; // Zeitpunkt des letzten FS check (auf freien Speicherplatz, s. checkFilesystemSpace())
float    GVcumDist          = 0; // aufsummierte Distanz zwischen den Fixpunkten (bis zum Schreiben)
double   GVtotalDist        = 0; // gesamt gefahrene Strecke lt. GPS (ohne Simulation)
TinyGPSPlus GVgpsNew;            // GPS-Modul

//********** Pumpe **********:
float     GVmeterSincePump = 0;     // Meter seit letztem Pumpen, kann nicht überlaufen, selbst wenn Pumpen deaktiviert... ;-)
uint16_t  GVtotalPumpCount = 0;     // Summe aller Pumpaktionen seit Start
tPumpMode GVpumpMode = MODE_NORMAL; // aktueller Modus der Pumpe (Normal, Regen, Offroad, ...)

//********** other **********:
bool     GVmaintenanceMode = false;   // Wartungsmodus -> keine GPS Behandlung und kein Notfallprogramm, Pumpen nur, wenn Dauerpumpen eingestellt wird.
uint32_t GVshowMaintMode = 0;         // Zeitpunkt (millis) des letzten LED Signals (dass Wartungsmodus aktiv ist)

//**** Button **************:
uint32_t GVlastPressed = 0;
uint32_t GVmillisPressed = 0;
uint8_t  GVbuttonState = LOW;
uint8_t  GVlastButtonState = LOW;
 
biLedx   GVmyLedx(LED_GRUEN, LED_ROT, LOW);  // LOW = Anschaltsignal
biLedx   GVmyPumpx(PUMP_PIN);                // default HIGH = Anschaltsignal

oilerDisplay GVmyDisplay;

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
  GVmyLedx.on(LED_GRUEN); // LED an waehrend Initialisierung
  pinMode(BUTTON_PIN, OUTPUT);
  digitalWrite(BUTTON_PIN, LOW);
    
  Serial.begin(9600);
  Serial.println();

  GVmyDisplay.Init();
  GVmyDisplay.PrintMessage("Init", 10000);

  // schon mal was auf's Display bringen (wird erst in Loop angezeigt über Check()):
  GVmyDisplay.PrintModeStr(getPumpModeStr(GVpumpMode));
  GVmyDisplay.PrintMeter(getModeMeters(GVpumpMode));
  GVmyDisplay.PrintKmh(0);

  if (!_FILESYS.begin()) {
    Serial.println(F("Fehler bei Initialisierung des Dateisystems!"));
  }

  GVoilerConf.read();
  GVmyDisplay.MessageAdd(".", 10000);
  GVcurrentfpw = GVoilerConf.fpw;  // merken, GVcurrentfpw bestimmt je nach Platz im FS, ob Tracks geschrieben werden...

  // Logging in Datei und Serial (sonst als 2. Parameter: false)
  // VORSICHT: falls Datei sehr groß wird, können Schreiboperationen (incl.
  // Öffnen und schließen der Datei seeehr langsam sein und das ganze Ding voll ausbremsen - also nur kurz
  // aktiviert lassen (ein paar Stunden oder sogar 1-2 Tage geht aber schon)!
  if (!myLogger.begin(GVoilerConf.lgf ? "/myLogger.txt" : "", GVoilerConf.lgs)) {
    Serial.println(F("Fehler bei Initialisierung des (LittleFS) Logger!"));
  }
  GVmyDisplay.MessageAdd(".", 10000);
  
  DEBUG_OUT.println(F("********** Starting..."));

  setupButton();

  //TODO: +++hier könnte noch ein Check auf Track-Dateien < x bytes erfolgen - die dann löschen

  checkforUpdate();
  GVmyDisplay.MessageAdd(".", 10000);

  checkFilesystemSpace(); // tracking deaktivieren, wenn zu wenig Platz
  GVmyDisplay.MessageAdd(".", 10000);

  // WiFi wird beim Starten ausgeschaltet und kann bei Bedarf (Button) angeschaltet werden:
  WiFi.forceSleepBegin(); // Wifi off
  GVwifiSleeping = true;

  setupGPS(); // Abfrage auf Wartungsmodus innerhalb der Funktion
  GVmyDisplay.MessageAdd(".", 10000);

  setupWebServer();
  GVmyDisplay.MessageAdd(".", 10000);

  checkTank();  // Öltank leer?

  DEBUG_OUT.print(F("This is wifiOiler version "));
  DEBUG_OUT.print(VERSION);
  DEBUG_OUT.print(F(" on board "));
  DEBUG_OUT.println(BOARD_TYPE);

  if (GVoilerConf.wso || GVmaintenanceMode) 
    toggleWiFi();
  else 
    GVmyLedx.off();
  GVmyDisplay.PrintMessage("wifiOiler\n  v");
  GVmyDisplay.MessageAdd(VERSION,2000);
  #ifdef _NO_PUMP_
    DEBUG_OUT.println(F("ACHTUNG: Pumpe dauerhaft deaktiviert!!"));
    GVmyDisplay.PrintAckMessage("Pumpe\ndeaktiv.\n(confirm)");
  #endif
  DEBUG_OUT.println(F("-------- Initialisierung beendet."));
}

/*******************************************************************
 ***************** Main Loop ***************************************
 *******************************************************************/
void loop() {
  if (!GVwifiSleeping) {
    MDNS.update();
    GVdnsServer.processNextRequest();
    GVwebServer.handleClient();
    checkWiFi();
  }
  GVmyLedx.checkState();  // LED-Behandlung
  GVmyPumpx.checkState(); // Behandlung Pumpe
  checkButton();        // Button Abfrage  
  checkGPSdata();       // GPS Daten lesen, falls welche da sind
  checkMovement();      // Überprüfung auf Bewegung
  GVmyDisplay.Check();    // Display handling
  
  if (GVmaintenanceMode && (millis() > (GVshowMaintMode + 10000UL))) {
    // noch ne kleine Spezialroutine, um Wartungsmodus anzuzeigen:
    GVmyLedx.start LED_SHOW_MAINTENANCE;
    GVshowMaintMode = millis();
  }
  if (GVupdateResult == UPD_REBOOT_NOW) {
    checkforUpdate();
    GVupdateResult = UPD_NOT_ACTIVE; // falls er wieder zurückkommt...
  }
}
