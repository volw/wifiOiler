/**
 * Changelog:
 * 1.03: Beim Schreiben eines GPS Fixes wird die LED nur dann ausgelöst, wenn sie NICHT aktiv ist
 * 1.10: Wenn zu wenig Platz im Filesystem ist, wird Tracking deaktiviert (s. checkFilesystemSpace())
 * 1.11: 1.10 war nicht richtig implementiert (millis() abziehen bei Zeitprüfung)
 *     : checkFilesystemSpace() in Web-Handler eingefügt, die Einfluss auf Speicherplatz haben (upload, delete, etc.)
 * 1.21: Update vom Web-Server implementiert
 * 1.22: Bug with file system check (GVcurrentfpw introduced)
 * 1.23: waiting page (update) improved
 * 1.24: handle reboot by myself (not automatic by ESP8266httpUpdate) - improved user feedback
 * - released -
 * 1.25: LED Blinksignale als #define, dann übersichtlicher einstellbar
 * 1.26: Update vom Web-Server abhängig vom Board und der eigenen Version (!)
 * 1.30: implemented another biled object (biLedx) with more fancy functionality ;-)
 * 1.32: Bug-Fixing biLedx
 * - released -
 * 1.40: SPIFFSlogger introduced
 * 1.41: minor corrections
 * 1.42: logging to file and/or serial (or no logging)
 * 1.43: MODE_RAIN -> MODE_REGEN; pumpMode.htm völlig neu - mit Abfrage des aktuellen Modus'
 * 1.44: new config method (no script injection anymore - but http request to get values)
 * 1.45: configuration.lin Parameter entfernt (da no script injection anymore)
 * 1.46: get version via http request for index page
 * 1.47: simplified config save
 * 1.48: pumpmode function aktualisiert
 * 1.49: Some minor web optimizations
 * 1.50: Delete Option in File Browser
 * 1.51: Logger Options (file and/or Serial)
 * 1.52: update optimized
 * 1.53: better feedback while uploading tracks
 * ---------------- major change: changed MCU, PullDown PumpPin Resistor, PullDown ButtonPin Resistor
 * 1.60: Better Button- and Pump Handling (with new circuit)
 * 1.61: Update auf "esp8266 by ESP8266 Community Version 2.6.3" (updated from 2.5.2)
 * 1.70: ModemSleep nach Karenzzeit und bei Bewegung eingeführt (LF33CV kühlt merklich ab)
 * 1.71: Better Upload (File search) Handling (SPIFFS.openDir("/2")).
 * 1.72: changed filename handling (starting with millenium (20) and cut seconds at end) and changed to SPIFFS.openDir("/20").
 * 1.73: new GPS logging option 
 * 1.74: Dauerpumpen nach x mal abschalten, wenn Kiste sich bewegt
 * 1.75: optimized moving detection
 * 1.76: WiFi auf Knopfdruck eingeführt
 * 1.77x: code optimizations
 * 2.01: Display introduced
 * 2.02: added client reference to http.begin() calls; replaced button routines (ISR and checkButton)
 * 4.1.002: changed version numbering - sync with board version. First digit: Major Board/Source Version, Second: Board Sub-version, Third: Source Sub-version
 * 4.1.003: changed from ISR for button to loop (much more reliable)
 * 4.2.000: Moved from SPIFFS to LittleFS
 * 4.2.001: per define kann Dateisystem ausgewählt werden (_SPIFFS_ oder _LITTLEFS_)
 * 4.2.002: nach Buttondruck wird mögl. Pumpvorgang nach Karenzzeit von 1,5 s ausgelöst. Grund: möchte man von 'Offroad' auf
 *          'Normal' durchschalten, geht man immer über 'Permanent' und dann wird gleich gepumpt. Das soll verhindert werden.
 * 4.2.003: Update Oiler: Prüfung auf Server Verfügbarkeit eingefügt (mit entsprechender Meldung, falls nicht).
 * 4.2.004: Initialsisierung des Screens ausgelagert (eigene Init() Funktion)
 * 4.2.005: own littlefs browser with simple editor introduced
 * 4.2.006: file system browser with editor as configuration item (s. config)
 * 4.2.007: get rid of httpUpdate()
 * 4.2.008: sending GVupdateMessage on UPDATEOK and REBOOT
 * 4.2.009: code completely restructured
 * 4.2.010: ota update optimized
 * 4.2.011: reboot routine renewed, new reboot.htm
 * 
 * wishlist (x = erledigt)
 * --------
 * - der Öler verschluckt manchmal Ölvorgänge, ggf. weil er mit anderen Dingen beschäftigt ist? nicht so gut
 * - GPS Module konfigurieren (config files)
 * - Logs hochladen und löschen
 * - Datum und Zeit im Log verwenden (wenn ich's denn vom GPS bekommen habe)
 * x Error handling beim Update... (s. updError in webHandler/handleUpdate())
 * x es werden immer noch ca. 6 Minuten bis zum ersten Fix benötigt - das ist etwas zu lang...
 *   erledigt: WiFi ist am Anfang ausgeschaltet und stört nicht mehr den GPS Empfang
 * x Wartefenster bei Softboot? besseres Handling wäre schön...
 * x GPS Kennsätze sollten ggf. flexibel konfiguriert/erkannt werden.
 * x Abstellen des Dauerpumpens nach x mal (wenn sich Motorrad bewegt)
 * x ggf. flexibel GPS Logging ein-/ausschalten können...
 * x Ausschaltbedingungen für WiFi sind noch nicht optimal... (isMoving())
 * x Blinkt zu früh grün - 10s sind dann noch nicht vorbei (muss auch nicht mehr, da Button Problematik gelöst)
 * x beim Einschalten wird die Pumpe betätigt - nicht so schön!
 *   (erledigt mit 10K Widerstand gegen GND, wird also auf LOW gezogen, wo es auch hingehört)
 * x (erledigt) WiFi könnte bei Bewegung des Bikes abgeschaltet werden, um Strom zu sparen 
 *   und die Hitzeentwicklung des Spannungswandlers zu reduzieren
 *   Dies könnte mit einem "Modem-Sleep" erreicht werden (WiFi.forceSleepBegin() =  Wifi off und
 *   WiFi.forceSleepWake() = Wifi on (muss danach die Setup Routine wieder laufen?)
 * x Anschluss für Arduino Programmierung
 *   (erledigt: NEO wird gesteckt, der gleiche Anschluss kann für Serial Adapter verwendet werden!!)
 *   (während der Programmierung wird das GPS-Modul halt entfernt)
 * x Button Problematik: wird die Maschine gezündet, wird der MCU ggf. kurz Strom entzogen, ohne komplett
 *   neu zu booten. Genug, um den Button Pin kurz gegen LOW zu ziehen und (wenn ISR bei Falling) ausgelöst
 *   wird und Button normal auf High ist - dann wird damit unerwünscht ein Buttondruck "simuliert"
 *   Abhilfe: Abfrage umdrehen - Button immer auf LOW lassen (PullDown Widerstand) und auf HIGH abfragen, 
 *   kann somit nicht versehentlich "fallen". Der GPIO16 hat angeblich einen eingebauten PullDown Widerstand, 
 *   den ich aber nicht nutzen möchte - externe PullDown oder -Up Widerstände scheinen mir zuverlässiger.
 *   (Quelle: https://tttapa.github.io/ESP8266/Chap04%20-%20Microcontroller.html)
 *   "GPIO 0-15 all have a built-in pull-up resistor, just like in an Arduino. GPIO16 has a built-in pull-down resistor."
 *   Nachtrag: ein Test mit GPIO16 hat nicht funktioniert. Tastendrücke wurden NICHT erkannt... (
*/
#ifndef _wifioiler_h_
#define _wifioiler_h_

// VERSION: für Update Vorgang, Version wird auch auf Startseite des Ölers angezeigt.
// _DISPLAY_AVAILABLE_: wenn undefiniert, wird etwas Speicherplatz (7 KB) gespart - ansonsten keine Auswirkung (auch wenn kein Display angeschl. ist)
// _NO_PUMP_: nur während Entwicklung sinnvoll: Pumpe wird dauerhaft deaktiviert, damit's nicht ständig piept
// _SPIFFS_: verwendetes Dateisystem ist SPIFFS, Dateien müssen neu hochgeladen werden
// _LITTLEFS_: verwendetes Dateisystem ist LittleFS, Dateien müssen neu hochgeladen werden

#define VERSION "4.2.011"
#define _DISPLAY_AVAILABLE_
//#define _NO_PUMP_
//#define _SPIFFS_
#define _LITTLEFS_

#define DEBUG_OUT myLogger
//#define DEBUG_OUT Serial

//#define LED_BUILTIN 2

//max. Zeichen pro GPS Record
#define GPS_BUFFER_LENGTH 90
#define DNS_PORT 53

// GPS-Modul an serieller Schnittstelle angeschlossen:
// SoftwareSerial funktionierte nicht so gut, könnte hier aber einfach umgestellt werden:
#define gpsSerial Serial

// falls im Filesystem weniger Platz übrig ist, um MIN_MINUTES_FREE Fahrzeit aufzuzeichnen, wird tracking deaktiviert...
// wird bei Start des Oilers, Sichern der Konfiguration und regelmäßig in writeGPSInfo() geprüft.
// zusätzl. nun auch bei allen webhandlern, die Einfluss auf Speicherplatz haben (upload, delete, track upload, etc.)
#define MIN_MINUTES_FREE 10

// Einstellungen für Pumpe (Dellorto getestet): Ein Impuls von 200ms Länge, dann 500ms Pause
#define PUMP_ON_DURATION 200
#define PUMP_OFF_DURATION 500

// ist der vorherige Fix-Punkt älter als MAX_OLD_LOCATION_AGE (in ms), wird keine Entfernung berechnet
#define MAX_OLD_LOCATION_AGE 10000UL

// max. Größe des GPS Config Buffers:
#define MAX_GPS_CONFIG_COMMAND 96

// ********** Movement detection:
// die Kiste bewegt sich, wenn innerhalb der letzten MAX_MOVEMENT_COUNT Sekunden
// mindestens MIN_MOVEMENT_POSITIV Bewegungen per GPS gemessen wurden (Routinen in utils.h)
// - void checkMovement()
// - bool isMoving()
#define MAX_MOVEMENT_COUNT 30
#define MIN_MOVEMENT_POSITIV 20
#define MAX_MOVEMENT_NEGATIVE 15
// wenn Dauerpumpen eingestellt wird, wird nach x mal
// die Pumpe abgestellt, wenn die Karre sich bewegt
#define MAX_PUMP_ACTION_WHEN_MOVING 10

// ********** Pumpen-Modi:
// enums are pretty much like #defines (Arduino specific)
enum PumpModes {
  MODE_OFF,             // Pumpe deaktiviert
  MODE_NORMAL,          // Normal Mode, s. Config.nmm
  MODE_REGEN,           // Regen Modus, s. Config.rmm
  MODE_OFFROAD,         // Offroad Modus, s. Config.omm
  MODE_PERMANENT        // Dauerpumpen
};
typedef PumpModes tPumpMode;

// ********** für Button Handling:
#define DEBOUNCE_MS 100UL
// wie lange muss Button gedrückt werden, um als lang gedrückt erkannt zu werden (1,5s):
#define BUTTON_LONG_DURATION 1200

// Überblick aller Blinksignale (hier einstellen):
// 1. LED Pin, 2. Dauer ON (default=600), 3. Dauer OFF (default=400), 4. Anzahl (default=1), 
// falls kein Blinken erwünscht, einfach leere Klammer:
#define LED_NO_BLINKING          ()
#define LED_SHOW_MAINTENANCE     (LED_GRUEN,  50,  50,  5)
#define LED_GPS_WRITE_FAILED     (LED_ROT  , 400, 200)
#define LED_GPS_WRITE_NO_TRACKS  (LED_ROT  , 400, 200)
#define LED_GPS_WRITE_SUCCESS    (LED_GRUEN, 400, 200)
#define LED_START_BUTTON_PRESSED (LED_GRUEN,  50,  50, 12)  // delaying / modal, also auch kurze Zeiten realisierbar
#define LED_WIFI_CONNECT_SUCCESS (LED_GRUEN,  50,  50,  3)
#define LED_WIFI_CONNECT_FAILED  (LED_ROT  ,  50,  50,  3)
#define LED_TRACK_UPLOAD_SUCCESS (LED_GRUEN,  50,  50,  3)  // vorsicht: delaying / modal
#define LED_TRACK_UPLOAD_FAILED  (LED_ROT  ,  50,  50,  3)  // vorsicht: delaying / modal
#define LED_FILE_UPLOAD_SUCCESS  (LED_GRUEN, 500,  50)      // vorsicht: delaying / modal
#define LED_SWITCH_MODE_RESPONSE (LED_GRUEN, 300, 200, GVpumpMode)
#define LED_SWITCH_MODE_OFF      (LED_ROT,   300, 200)
#define LED_TEST_GRUEN           (LED_GRUEN, 200, 100,  3)
#define LED_TEST_ROT             (LED_ROT  , 200, 100,  3)
#define LED_SHOW_PUMP_ACTION     (LED_ROT, PUMP_ON_DURATION, PUMP_OFF_DURATION, GVoilerConf.pac)
#define LED_SHOW_WIFI_OFF        (LED_ROT  ,  50,  50,  3)
#define LED_SHOW_WIFI_ON         (LED_GRUEN,  50,  50,  3)

// Für Pumpe:
#define PUMP_ACTION              (PUMP_PIN, PUMP_ON_DURATION, PUMP_OFF_DURATION, GVoilerConf.pac)
// einige Feste LED Signale gibt es noch:
// während der Initialisierung leuchtet die LED grün
// wenn Öltank Leer, wird 10 mal abwechselnd (rot/grün) geblinkt (während Initialisierung)
// wenn Button lange gedrückt wird, leuchtet die LED rot (bis wieder losgelassen wird)


#define BUTTON_PIN  D6  // when D6 not "pulled down", start with connection between D6 and GND 
                        // (otherwise oiler will start in maintenance mode)
#define PUMP_PIN    D7

// NodeMCU ESP8266 und nackter ESP-12F
#ifdef ARDUINO_ESP8266_NODEMCU
  #define LED_ROT          D5   // weiße Litze vom Cockpit-Kabel
  #define LED_GRUEN        D0   // gelbe Litze vom Cockpit-Kabel
  #pragma message "Info: compiling for NodeMCU/ESP12F..."
  #define BOARD_TYPE       "NodeMCU"
  #define UPDATE_FILE      "/wifiOiler.ino.nodemcu.bin"  
#endif

// Wemos D1 mini
#ifdef ARDUINO_ESP8266_WEMOS_D1MINI
  #define LED_ROT          D5 // alt D7
  #define LED_GRUEN        D0 // alt D8
  #pragma message  "Info: compiling for Wemos D1 mini..."
  #define BOARD_TYPE       "WemosMini"
  #define UPDATE_FILE      "/wifiOiler.ino.d1_mini.bin"  
#endif

#ifndef BOARD_TYPE
  #error "Board nicht spezifiziert (s. wifiOiler.h)"
#endif

#ifdef _NO_PUMP_
  #warning "Achtung: Pumpe ist deaktiviert - s. wifiOiler.h, define _NO_PUMP_"
#endif

#ifdef _SPIFFS_
  #ifdef _LITTLEFS_
    #error "_SPIFFS_ und _LITTLEFS_ sind beide definiert (es darf nur einen geben, s. wifiOiler.h)"
  #else
    #define _FILESYS SPIFFS
    #include <FS.h>
  #endif
#else
  #ifdef _LITTLEFS_
    #define _FILESYS LittleFS
    #include <LittleFS.h>
  #else
    #pragma message "Dateisystem nicht spezifiziert, LittleFS wird angenommen"
    #define LittleFS
    #define _FILESYS LittleFS
    #include <LittleFS.h>
  #endif
#endif

// file system defines check:
#ifndef _FILESYS
  #error "Kein Dateisystem spezifiert. Siehe _FILESYS in wifiOiler.h"
#endif

//**************************************************************
//***** forward declarations for all functions *****************
//**************************************************************
bool     analyzeGPSInfo(void);
void     checkButton(void);
void     checkFilesystemSpace(void);
bool     checkforUpdate(bool justCheck = false, bool reboot = true); // default (false, true)
void     checkGPSdata(void);
void     checkMovement(void);
void     checkTank(void);
void     checkWiFi(void);
void     configureGPS(void);
bool     createDateFilename(uint32_t date, uint32_t time);
uint8_t  deleteUpdateFiles(void);
bool     downloadFile(String subPath);
bool     evalGPS(char data);
String   getContentType(String fname);
uint16_t getModeMeters(tPumpMode mode);
String   getPumpModeStr(tPumpMode nMode);
bool     getUpdateInfo(void);
void     handleConfigPage(void);
void     handleDebugInfo(void);
void     handleFileCreate(void);
void     handleFileDelete(void);
void     handleFileList(void);
bool     handleFileRead(String path);
void     handleFileUpload(void);
void     handleLEDTest(void);
void     handleMessage(String message, bool justBack = true);
void     handlePumpMode(void);
void     handlePumpTest(void);
void     handleReboot(void);
void     handleUpdate(void);
bool     handleUpdateFiles(void);
void     handleUpload(void);
void     handleVersion(void);
bool     InitiatePump(void);
bool     isFileThere(String fname);
bool     isGGArecord(void);
bool     isHalting(void);
bool     isMoving(void);
bool     isRMCrecord(void);
bool     isServerAvailable(void);
uint8_t  readWifiData(void);
bool     renameUpdateFiles(void);
bool     sendFile(String fname);
void     setNewMode(tPumpMode newMode);
void     setupButton(void);
void     setupGPS(void);
void     setupMDNS(void);
void     setupWebServer(void);
bool     setupWiFi(void);
uint8_t  simMeters(void);
void     toggleWiFi(void);
void     writeGPSInfo(float dist);

#endif
