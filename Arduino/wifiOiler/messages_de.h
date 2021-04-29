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

// general
#define MSG_GLOBAL_YES "Ja"
#define MSG_GLOBAL_NO  "Nein"

// wifiOiler.ino
#define MSG_ERROR_INITIALISING_FILESYSTEM "Fehler bei Initialisierung des Dateisystems!"
#define MSG_ERROR_INITIALISING_LOGGER     "Fehler bei Initialisierung des (LittleFS) Logger!"
#define MSG_PROGRESS_STARTING_INIT        "********** Starting..."
#define MSG_PROGRESS_STARTING_END         "-------- Initialisierung beendet."
#define MSG_VERSION_MESSAGE_1             "wifiOiler, Version "       // followed by version + MSG_VERSION_MESSAGE_2
#define MSG_VERSION_MESSAGE_2             " auf MCU "                 // followed by board type
#define MSG_ATT_PUMP_DEACTIVATED          "ACHTUNG: Pumpe dauerhaft deaktiviert!!"

// FSBrowser
#define MSG_DBG_FS_FREE_SPACE_INFO        "[checkFilesystemSpace] LittleFS free: %d bytes = %d GPS records (%dh:%02d)\n" // freespace, gpsrec, gpsmin/60, gpsmin%60
#define MSG_DBG_RECORDING_STOPPED         "[checkFilesystemSpace] track Aufzeichnung gestoppt"
#define MSG_DBG_RECORDING_CONTINUED       "[checkFilesystemSpace] track Aufzeichnung fortgesetzt"
#define MSG_HTML_TARGET_FILE_EXISTS       "Zieldatei existiert bereits"
#define MSG_HTML_SOURCE_FILE_NOT_EXISTS   "Quelldatei existiert nicht"
#define MSG_HTML_ERROR_RENAMING_FILE      "FEHLER: Datei konnte nicht umbenannt werden"
#define MSG_HTML_ERROR_CREATING_FILE      "FEHLER: Datei konnte nicht erstellt werden"
#define MSG_DBG_HANDLEFILEDELETE_PATH     "[handleFileDelete] "       // followed by path/name of file to delete
#define MSG_DBG_HANDLEFILEUPLOAD_URI      "[handleFileUpload] GVwebServer.uri() != /edit"
#define MSG_DBG_HANDLEFILEUPLOAD_NAME     "[handleFileUpload] Name: " // followed by (upload) filename
#define MSG_DBG_HANDLEFILEUPLOAD_SIZE     "[handleFileUpload] Größe: " // followed by total (upload) file size

// GPS
#define MSG_DBG_SETUP_GPS_INIT            "[setupGPS] GPS Modul wird initialisiert..."
#define MSG_DBG_GPS_SWITCH_OFF_PUMP       "[checkGPSdata] Dauerpumpen wird ausgeschaltet..."
#define MSG_DBG_GPS_DATE_TIME             "[createDateFilename] Datum/Zeit: " // followed by recently created track file name
#define MSG_DBG_MOVEMENT_MOTO_MOVING      "[checkMovement] Info: Fahrzeug bewegt sich jetzt."
#define MSG_DBG_MOVEMENT_MOTO_STOPPED     "[checkMovement] Info: Fahrzeug wurde gestoppt."

// WiFi
#define MSG_DBG_ADD_WIFI_TO_MULTIWIFI     "[readWifiData] folgendes WLAN wird gesucht: " // followed by SSID from file "/wifi.ini"
#define MSG_DBG_START_SEARCHING_WIFI      "[setupWiFi] suche WLAN.."
#define MSG_DBG_TRY_CONNECT_WIFI          "[setupWiFi] versuche, mit bekanntem WLAN zu verbinden.."
#define MSG_DBG_WIFI_NOT_CONNECTED        "[setupWiFi] Keine WLAN Verbindung"
#define MSG_DBG_NO_WIFI_CONFIGS_FOUND     "[setupWiFi] Keine WLAN Konfiguration(en) gefunden"
#define MSG_DBG_CONNECT_SUCCESS           "[setupWiFi] verbunden mit '%s', IP:%s\n"  // printf(), SSID, local IP
#define MSG_DBG_START_ACCCESS_POINT       "[setupWiFi] Erstelle WLAN Access Point '%s' - Kennwort: '%s'\n"	// printf()
#define MSG_DBG_ACCESS_POINT_IP           "[setupWiFi] IP-Adresse: " // followed by Access Point IP
#define MSG_DBG_ACCESS_POINT_FAILED       "[setupWiFi] FEHLER: WLAN Access Point konnte nicht erstellt werden!!"
#define MSG_DBG_MDNS_ERROR                "[setupMDNS] FEHLER: Initialisierung des mDNS Service fehlgeschlagen"
#define MSG_DBG_MDNS_STARTED              "[setupMDNS] mDNS Service gestartet, verbinde mit 'http://%s.local'\n"
#define MSG_DBG_STARTING_WIFI             "[toggleWiFi] starte WiFi ..."
#define MSG_DBG_STOPPING_WIFI             "[toggleWiFi] stoppe WiFi..."

// button
#define MSG_DBG_BUTTON_PRESSED_ON_START   "[setupButton] Button während des Starts gedrückt.."
#define MSG_DBG_ENTER_MAINTENANCE_MODE    "[setupButton] Wartungsmodus wird gestartet.."
#define MSG_DBG_BUTTON_LONG_PRESSED       "[checkButton] WiFi-Umschaltung ausgelöst.."
#define MSG_DBG_BUTTON_SHORT_PRESSED      "[checkButton] Aktivierung des nächsten Pumpenmodus.."

// oiler
#define MSG_DBG_NEW_PUMP_MODE             "Neuer Pumpemmodus ist >%s< - nächster Pumpvorgang in (Meter): " // printf: no eol, followed by meters
#define MSG_DBG_PUMP_DEACTIVATED          "(Pumpe deaktiviert)"
#define MSG_DBG_OIL_TANK_EMPTY            "[checkTank] Tank leer? - Öl nachfüllen und Zähler zurücksetzen (s. Konfiguration)"
#define MSG_DBG_TRIGGER_PUMP              "[TriggerPump] pumpen nach (s): %d, Pumpenmodus ist >%s<\n"
#define MSG_DBG_ERROR_OPEN_OILCNT_FILE    "[TriggerPump] Fehler beim Öffnen der OilCounter Datei"

// update
#define MSG_DBG_UPDATE_INIT               "[checkforUpdate] suche Firmware Datei " // followed by name of firmware file
#define MSG_DBG_UPDATE_START              "[checkforUpdate] Firmware gefunden - starte Update.."
#define MSG_DBG_UPDATE_ERROR              "[checkforUpdate] FEHLER"
#define MSG_DBG_UPDATE_RESULT             "[checkforUpdate] Update.end(true)==" // followed by Update.end(true)
#define MSG_DBG_UPDATE_GETERROR           "[checkforUpdate] Update.getError():" // followed by Update.getError()
#define MSG_DBG_REMOVING_FIRMWARE_FILE    "[checkforUpdate] lösche Firmware Datei " // followed by name of firmware file
#define MSG_DBG_ERROR_DEL_FIRMWARE_FILE   "[checkforUpdate] FEHLER: löschen der Firmware Datei fehlgeschlagen!!"
#define MSG_DBG_BOOT_AFTER_UPDATE         "[checkforUpdate] Neustart nach Update.."
#define MSG_DBG_NO_FIRMWARE_FILE          "[checkforUpdate] keine neue Firmware gefunden"
#define MSG_DBG_UPDATE_SERVER_URL         "[getUpdateInfo] URL = %s\n"	// printf
#define MSG_DBG_UPD_INFO_GET_RET_CODE     "[getUpdateInfo] http.GET() response: %d\n" // printf
#define MSG_DBG_UPD_INFO_GET_RET_TEXT     "[getUpdateInfo] http.GET() returned: " // followed by result of http.getString()
#define MSG_DBG_UPD_INFO_GET_ERROR        "[getUpdateInfo] http.GET() Fehler: %s\n" // printf, %s = http.errorToString(httpCode)
#define MSG_DBG_UPD_DOWNLOAD_INFO         "[downloadFile] downloading '%s' to '%s'\n" // printf, source, target
#define MSG_DBG_UPD_DOWNLOAD_RET_CODE     "[downloadFile] GET return code: %d\n" // printf
#define MSG_DBG_UPD_DOWNLOAD_OPEN_ERROR   "[downloadFile] Fehler beim Öffnen der Datei zum Schreiben!"
#define MSG_DBG_UPD_DOWNLOAD_GET_ERROR    "[downloadFile] http.Get() Fehler: %s\n" // printf, http.errorToString(httpCode)
#define MSG_DBG_UPD_DOWNLOAD_COMPLETE     "[handleUpdate] Download abgeschlossen:" // followed by filename
#define MSG_DBG_UPD_DOWNLOAD_ERROR        "[handleUpdate] FEHLER: Download fehlgeschlagen"
#define MSG_DBG_RENAME_FILE_INIT          "[renameUpdateFiles] fname = " // followed by name of update file
#define MSG_DBG_OLD_FILE_REMOVE_ERROR     "[renameUpdateFiles] FEHLER beim Löschen von "  // followed by .old file
#define MSG_DBG_OLD_FILE_RENAME_ERROR     "[renameUpdateFiles] FEHLER bei Umbenennung von '%s' in '%s.old'\n" // printf, source, target
#define MSG_DBG_TMP_FILE_RENAME_ERROR     "[renameUpdateFiles] FEHLER bei Umbenennung von '%s$' in '%s'\n" // printf, source (tmp), target
#define MSG_DBG_UPD_FILE_DELETE_INIT      "[deleteUpdateFiles] lösche "  // followed by name of file to delete
#define MSG_DBG_UPD_CONNECT_INIT          "[handleUpdate] suche Update Server\n"
#define MSG_HTTP_UPD_SERVER_CONNECT_ERROR "Update Server nicht erreichbar!"
#define MSG_DBG_GET_UPDATE_INFO           "[handleUpdate] lese Update Infomationen vom Server " // followed by server name
#define MSG_HTTP_NO_UPDATE_AVAILABLE      "Server meldet: kein Update vorhanden!"

// upload
#define MSG_DBG_CHECK_URL_START           "[isServerAvailable] check URL (GET):%s\n"  // printf: complete url (server, port and url)
#define MSG_DBG_CHECK_URL_SERVER_IP       "[isServerAvailable] IP-Adresse von '%s' ist %s\n"  // printf: server, ip
#define MSG_DBG_CHECK_URL_ERROR           "[isServerAvailable] GET() failed, error: %s\n" // printf: error message from get()
#define MSG_DBG_FILECHECK_RESULT          "[isFileThere] httpCode = %d\n" // printf: http-code returned
#define MSG_DBG_SEND_FILE_START           "[sendFile] Starte Übertragung der Datei..."
#define MSG_DBG_SEND_FILE_CONTENT         "[sendFile] >>> sending file content >>>"
#define MSG_DBG_SEND_FILE_COMPLETED       "[sendFile] Übertragung beendet"
#define MSG_DBG_SEND_FILE_CONNECT_ERROR   "[sendFile] Fehler beim Öffnen der Verbindung zu: '%s:%d'\n"  // printf: host name, port
#define MSG_DBG_SEND_FILE_FOPEN_ERROR     "[sendFile] Fehler beim Öffnen der Datei: %s\n" 	// printf: file name
#define MSG_DBG_TRACK_UPLOAD_START        "[handleUpload] searching track files:"
#define MSG_DBG_TRACK_FILE_FOUND_YES      " is a track file - will upload..."  // before this the file name is printed
#define MSG_DBG_TRACK_FILE_FOUND_NO       " is NO track file..."  // before this the file name is printed
#define MSG_DBG_UPLOAD_SERVER_ERROR       "[handleUpload] Upload Server not available!"

// webhandler
#define MSG_DBG_WEB_FILE_READ_ERROR       "[handleFileRead] FEHLER beim Lesen der Datei " // followed by file name (path)
