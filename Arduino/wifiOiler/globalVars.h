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

// häufiger verwendete Token (spart ein wenig Speicherplatz):
const char PROGMEM * TEXT_PLAIN = "text/plain";
const char PROGMEM * TEXT_HTML = "text/html";
const char PROGMEM * BAD_PATH = "BAD PATH";
const char PROGMEM * BAD_ARGS = "BAD ARGS";

// GVgpsNew.date.value()..........: uint32_t   4
// GVgpsNew.time.value()..........: uint32_t   4
// GVgpsNew.location.lat()........: double     8
// GVgpsNew.location.lng()........: double     8
// GVgpsNew.hdop.value()..........: float      4
// GVgpsNew.speed.kmph()..........: float      4
// GVgpsNew.altitude.meters().....: float      4
// GVgpsNew.location.age()........: uint32_t   4
// millis()-GVoldLocationAge......: uint32_t   4
// dist...........................: float      4
// GVmeterSincePump...............: float      4
// getModeMeters(GVpumpMode)......: uint16_t   2
// SUMME..........................:           54
// also pro MB ca. 15 Stunden Fahrzeit

// der Compiler berücksichtigt Hardware Optimierung
// deshalb wird sizeof(gpsData) nicht 54 ausgeben, sondern 56 (8-byte boundary).
// Ist man auf genaue Angaben angewiesen, muss das Attribut packed verwendet werden:
// struct __attribute__((packed)) gpsData {...}
// das geht u.a. allerdings auf die Performance, deshalb sorgen wir manuell für die Größe:
#define GPS_RECORD_SIZE 54

struct gpsData
{
  uint32_t datum;   // 4
  uint32_t zeit;    // 4
  double   loclat;  // 8
  double   loclng;  // 8
  float    hdop;    // 4
  float    speed;   // 4 
  float    alt;     // 4
  uint32_t locage;  // 4
  uint32_t oldage;  // 4
  float    dist;    // 4
  float    msp;     // 4 (Meter since pump)
  uint16_t pmode;   // 2
};                 // 54 Bytes

union gpsUnion  // Struktur gpsData wird hier über das char array gelegt
{
  char buffer[GPS_RECORD_SIZE];
  gpsData data;
};

gpsUnion GVgps; // global, no need to allocate each time
const char PROGMEM * GPSLogFile = "/gpslog.txt";

const char PROGMEM * wifiFile = "/wifi.ini";
const IPAddress myIP(192, 168, 4, 1);

//**** Update Oiler (upload files and restart on new binary ***************
#define UPD_NOT_ACTIVE       0
#define UPD_UPLOADING_FILES  1
#define UPD_UPDATE_ENDED     2
#define UPD_MUST_REBOOT      3
#define UPD_REBOOT_NOW       4
#define UPD_ERROR            5
String   GVupdateMessage    = "";     // response string for web gui
uint8_t  GVupdateResult     = UPD_NOT_ACTIVE;   // state of update (s. defines)
uint32_t GVupdFileSizeTotal = 0;     // upload progress, total size (bytes) of current file
uint32_t GVupdSizeUploaded  = 0;     // upload progress, uploaded number of bytes of current file
bool     GVupdError         = false; // some error during uploading process (file possibly not complete)
#define _UF_INITIAL  0
#define _UF_UPLOADOK 1
#define _UF_UPLOAD_ERROR 2
struct updateFile {
  String fileName;
  uint8_t state;
};
std::vector<updateFile> GVupdateFiles;

HTTPClient GVhttp;
WiFiClient GVwifiClient = GVwebServer.client();

// for track file upload
#define HEADER_LENGTH 195
const String boundary = "wifiOiler-dea88064e2e3320a";

const char PROGMEM * C_CONNECTTEST    = "Microsoft Connect Test";
const char PROGMEM * C_HOTSPOT_DETECT = "<html><script type='text/javascript'>window['_gaUserPrefs']={ioo:function(){return true;}}</script><head><title>Success</title><style></style></head><body>Success</body></html>";
const char PROGMEM * C_SERVE_INLINE   = "[GVwebServer.onNotFound] serve inline: ";
const char PROGMEM * C_FILENOTFOUND   = "[GVwebServer.onNotFound] not found: ";
const char PROGMEM * C_SERVEFILE      = "[GVwebServer.onNotFound] serve: ";

LittleFSlogger GVmyLogger;  // Serial und/oder in Datei
