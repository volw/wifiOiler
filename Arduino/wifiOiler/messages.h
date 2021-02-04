/****
 * wifiOiler, an automatic distance depending motorbike chain oiler
 * Copyright (C) 2019-2021, volw
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
 ****/

// wifiOiler.ino
#define MSG_ERROR_INITIALISING_FILESYSTEM "Fehler bei Initialisierung des Dateisystems!"
#define MSG_ERROR_INITIALISING_LOGGER     "Fehler bei Initialisierung des (LittleFS) Logger!"
#define MSG_PROGRESS_STARTING_INIT        "********** Starting..."
#define MSG_PROGRESS_STARTING_END         "-------- Initialisierung beendet."
#define MSG_VERSION_MESSAGE_1             "This is wifiOiler version " // followed by version + MSG_VERSION_MESSAGE_2
#define MSG_VERSION_MESSAGE_2             " on board "                 // followed by board type
#define MSG_ATT_PUMP_DEACTIVATED          "ACHTUNG: Pumpe dauerhaft deaktiviert!!"

// FSBrowser
#define MSG_HTML_TARGET_FILE_EXISTS       "Target file exists"
#define MSG_HTML_SOURCE_FILE_NOT_EXISTS   "Source file does'nt exist"
#define MSG_HTML_ERROR_RENAMING_FILE      "Could'nt rename file"
#define MSG_HTML_ERROR_CREATING_FILE      "Could'nt create file"
#define MSG_DBG_HANDLEFILEDELETE_PATH     "[handleFileDelete] "       // followed by path/name of file to delete
#define MSG_DBG_HANDLEFILEUPLOAD_URI      "[handleFileUpload] GVwebServer.uri() != /edit"
#define MSG_DBG_HANDLEFILEUPLOAD_NAME     "[handleFileUpload] Name: " // followed by (upload) filename
#define MSG_DBG_HANDLEFILEUPLOAD_SIZE     "[handleFileUpload] Size: " // followed by total (upload) file size

// GPS
#define MSG_DBG_SETUP_GPS_INIT            "[setupGPS] Initializing GPS module..."
#define MSG_DBG_GPS_CONFIG_FILE_NOT_EXIST "[setupGPS] ERROR: GPS Config file not found, check configuration: "	// followed by name of config file (see configuration)
#define MSG_DBG_GPS_NO_CONFIG_FILE        "[setupGPS] no GPS configuration file given"
#define MSG_DBG_GPS_SWITCH_OFF_PUMP       "[checkGPSdata] Dauerpumpen wird ausgeschaltet..."
#define MSG_DBG_GPS_DATE_TIME             "[createDateFilename] date/time: " // followed by recently created track file name
#define MSG_DBG_MOVEMENT_MOTO_MOVING      "[checkMovement] Info: Vehicle started moving."
#define MSG_DBG_MOVEMENT_MOTO_STOPPED     "[checkMovement] Info: Vehicle STOPPED!"

// WiFi
#define MSG_DBG_ADD_WIFI_TO_MULTIWIFI     "[readWifiData] Add wifi to wifiMulti: " // followed by SSID from file "/wifi.ini"
#define MSG_DBG_START_SEARCHING_WIFI      "[setupWiFi] suche bekanntes WLAN..."
#define MSG_DBG_WIFI_NOT_CONNECTED        "[setupWiFi] not connected to any WiFi"
#define MSG_DBG_NO_WIFI_CONFIGS_FOUND     "[setupWiFi] Keine WiFi Konfiguration(en) gefunden"
#define MSG_DBG_CONNECT_SUCCESS           "[setupWiFi] connected to '%s', IP:%s\n"  // printf(), SSID, local IP
#define MSG_DBG_START_ACCCESS_POINT       "[setupWiFi] Setting up access point '%s' - password '%s'\n"	// printf()
#define MSG_DBG_ACCESS_POINT_IP           "[setupWiFi] My IP address: " // followed by Access Point IP
#define MSG_DBG_ACCESS_POINT_FAILED       "[setupWiFi] Failed setting up access point!!"
#define MSG_DBG_MDNS_ERROR                "[setupMDNS] Error setting up mDNS responder!..."
#define MSG_DBG_MDNS_STARTED              "[setupMDNS] mDNS responder started, please connect to 'http://%s.local'\n"
#define MSG_DBG_STARTING_WIFI             "[toggleWiFi] starting WiFi ..."
#define MSG_DBG_STOPPING_WIFI             "[toggleWiFi] stopping WiFi..."

// button
#define MSG_DBG_BUTTON_PRESSED_ON_START   "[setupButton] Button pressed during start.."
#define MSG_DBG_ENTER_MAINTENANCE_MODE    "[setupButton] enter maintenance mode.."
#define MSG_DBG_BUTTON_LONG_PRESSED       "[checkButton] toggle WiFi requested..."
#define MSG_DBG_BUTTON_SHORT_PRESSED      "[checkButton] next pump mode requested..."