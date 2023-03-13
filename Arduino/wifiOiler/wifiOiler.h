/************************************************************************
 * wifiOiler, an automatic distance depending motorbike chain oiler
 * Copyright (C) 2019-2023, volw
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 ************************************************************************/
#ifndef _wifioiler_h_
#define _wifioiler_h_

// VERSION: für Update Vorgang, Version wird auch auf Startseite des Ölers angezeigt.
// _DISPLAY_AVAILABLE_: wenn undefiniert, wird etwas Speicherplatz (7 KB) gespart - ansonsten keine Auswirkung (auch wenn kein Display angeschl. ist)
// _NO_PUMP_: nur während Entwicklung sinnvoll: Pumpe wird dauerhaft deaktiviert, damit's nicht ständig piept
// _CAPTIVE_PORTAL_: Access Point wird als Captive Portal erstellt (wie Anmeldeseite eines Hotspots z.B. in Hotels)

#define VERSION "4.2.029"
#define _DISPLAY_AVAILABLE_
//#define _NO_PUMP_
#define _CAPTIVE_PORTAL_


#define LOG_FILE_NAME "/myLogger.txt"
// Log wird nicht "rund"-geschrieben - wenn größer (check beim Start), dann wird gelöscht!
#define LOG_FILE_MAX_BYTES 200000
#define DEBUG_OUT GVmyLogger
//#define DEBUG_OUT Serial
#define FS_BROWSER_URL "/littlefsb.htm"

// wird der HTTP_USER_AGENT geändert (Sonderzeichen vermeiden!), dann sollte auch in der 'oilerbase.php' (oilerbase)
// die Variable USER_AGENT entsprechend angepasst werden (s. oilerbase/html/oilerbase.php)
#define HTTP_USER_AGENT "wifiOiler"

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
// wie lange muss Button gedrückt werden, um als lang gedrückt erkannt zu werden:
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


#define BUTTON_PIN  D6
#define PUMP_PIN    D7
#define LED_ROT          D5   // weiße Litze vom Cockpit-Kabel
#define LED_GRUEN        D0   // gelbe Litze vom Cockpit-Kabel

// NodeMCU ESP8266 und nackter ESP-12F
#if defined ARDUINO_ESP8266_NODEMCU
  #pragma message "Info: compiling for NodeMCU/ESP12F..."
  #define BOARD_TYPE  "NodeMCU"  // case sensitive; used to build directory name on oilerbase
  #define UPDATE_FILE "/wifiOiler.ino.nodemcu.bin"  
#endif

// Wemos D1 mini
#if defined ARDUINO_ESP8266_WEMOS_D1MINI
  #pragma message  "Info: compiling for Wemos D1 mini..."
  #define BOARD_TYPE  "WemosMini"  // case sensitive; used to build directory name on oilerbase
  #define UPDATE_FILE "/wifiOiler.ino.d1_mini.bin"  
#endif

// Wemos D1 mini Pro
#if defined ARDUINO_ESP8266_WEMOS_D1MINIPRO
  #pragma message  "Info: compiling for Wemos D1 mini Pro..."
  #define BOARD_TYPE  "WemosMiniPro"  // case sensitive; used to build directory name on oilerbase
  #define UPDATE_FILE "/wifiOiler.ino.d1_mini.bin"  
#endif

#ifndef BOARD_TYPE
  #error "Board nicht spezifiziert (s. wifiOiler.h)"
  // andere Boards könnten auch funktionieren, sind aber nicht getestet...
#endif

#ifdef _NO_PUMP_
  #warning "Achtung: Pumpe ist deaktiviert - s. wifiOiler.h, define _NO_PUMP_"
#endif

#define _FILESYS LittleFS
#include <LittleFS.h>

//**************************************************************
//***** forward declarations ***********************************
//**************************************************************
// we have to do some forward declarations because default
// values for params are not working for 'normal' functions:
bool checkforUpdate(bool justCheck = false, bool reboot = true);

#endif
