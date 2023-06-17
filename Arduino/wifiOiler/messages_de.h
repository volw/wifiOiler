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

// general
#define MSG_GLOBAL_YES "Ja"
#define MSG_GLOBAL_NO  "Nein"

// wifiOiler.ino
#define MSG_ERROR_INITIALISING_FILESYSTEM "Fehler bei Initialisierung des Dateisystems!"
#define MSG_ERROR_INITIALISING_LOGGER     "Fehler bei Initialisierung des (LittleFS) Logger!"
#define MSG_CHECK_LOG_SIZE_DELETE         "Log-Datei war zu groß und musste gelöscht werden\n"
#define MSG_PROGRESS_STARTING_INIT        "********** Starting...\n"
#define MSG_PROGRESS_STARTING_END         "-------- Initialisierung beendet.\n"
#define MSG_VERSION_MESSAGE               "wifiOiler, Version %s auf MCU %s\n"        // version, board type
#define MSG_ATT_PUMP_DEACTIVATED          "ACHTUNG: Pumpe dauerhaft deaktiviert!!\n"

// FSBrowser
#define MSG_DBG_FS_FREE_SPACE_INFO        "LittleFS free: %d bytes = %d GPS records (%dh:%02d)\n" // freespace, gpsrec, gpsmin/60, gpsmin%60
#define MSG_DBG_RECORDING_STOPPED         "track Aufzeichnung gestoppt\n"
#define MSG_DBG_RECORDING_CONTINUED       "track Aufzeichnung fortgesetzt\n"
#define MSG_HTML_TARGET_FILE_EXISTS       "Zieldatei existiert bereits"
#define MSG_HTML_SOURCE_FILE_NOT_EXISTS   "Quelldatei existiert nicht"
#define MSG_HTML_ERROR_OPEN_SOURCE        "FEHLER: Quelldatei konnte nicht geöffnet werden"
#define MSG_HTML_ERROR_OPEN_TARGET        "FEHLER: Zieldatei konnte nicht geöffnet werden"
#define MSG_HTML_ERROR_RENAMING_FILE      "FEHLER: Datei konnte nicht umbenannt werden"
#define MSG_HTML_ERROR_CREATING_FILE      "FEHLER: Datei konnte nicht erstellt werden"
#define MSG_DBG_HANDLEFILEDELETE_PATH     "%s\n"       // followed by path/name of file to delete
#define MSG_DBG_HANDLEFILEUPLOAD_URI      "GVwebServer.uri() != /edit\n"
#define MSG_DBG_HANDLEFILEUPLOAD_NAME     "Name: %s\n" // followed by (upload) filename
#define MSG_DBG_HANDLEFILEUPLOAD_SIZE     "Größe: %d\n" // followed by total (upload) file size

// GPS
#define MSG_DBG_SETUP_GPS_INIT            "GPS Modul wird initialisiert...\n"
#define MSG_DBG_GPS_SWITCH_OFF_PUMP       "Dauerpumpen wird ausgeschaltet...\n"
#define MSG_DBG_GPS_DATE_TIME             "Datum/Zeit: %s\n"    // followed by recently created track file name
#define MSG_DBG_MOVEMENT_MOTO_MOVING      "Info: Fahrzeug bewegt sich jetzt.\n"
#define MSG_DBG_MOVEMENT_MOTO_STOPPED     "Info: Fahrzeug wurde gestoppt.\n"

// WiFi
#define MSG_DBG_ADD_WIFI_TO_MULTIWIFI     "folgendes WLAN wird gesucht: %s\n" // SSID from file "/wifi.ini"
#define MSG_DBG_START_SEARCHING_WIFI      "suche WLAN..\n"
#define MSG_DBG_TRY_CONNECT_WIFI          "versuche, mit bekanntem WLAN zu verbinden...\n"
#define MSG_DBG_WIFI_NOT_CONNECTED        "Keine WLAN Verbindung!\n"
#define MSG_DBG_NO_WIFI_CONFIGS_FOUND     "Keine WLAN Konfiguration(en) gefunden\n"
#define MSG_DBG_CONNECT_SUCCESS           "verbunden mit '%s', IP:%s\n"  // SSID, local IP
#define MSG_DBG_START_ACCCESS_POINT       "Erstelle WLAN Access Point '%s' - Kennwort: '%s'\n"
#define MSG_DBG_ACCESS_POINT_IP           "IP-Adresse: %s\n" // followed by Access Point IP
#define MSG_DBG_ACCESS_POINT_FAILED       "FEHLER: WLAN Access Point konnte nicht erstellt werden!!\n"
#define MSG_DBG_MDNS_ERROR                "FEHLER: Initialisierung des mDNS Service fehlgeschlagen\n"
#define MSG_DBG_MDNS_STARTED              "mDNS Service gestartet, verbinde mit 'http://%s.local'\n"
#define MSG_DBG_STARTING_WIFI             "starte WiFi ...\n"
#define MSG_DBG_STOPPING_WIFI             "stoppe WiFi...\n"

// button
#define MSG_DBG_BUTTON_STATE              "Button state == %s\n"
#define MSG_DBG_BUTTON_PRESSED_ON_START   "Button während des Starts gedrückt..\n"
#define MSG_DBG_ENTER_MAINTENANCE_MODE    "Wartungsmodus wird gestartet..\n"
#define MSG_DBG_BUTTON_LONG_PRESSED       "WiFi-Umschaltung ausgelöst..\n"
#define MSG_DBG_BUTTON_SHORT_PRESSED      "Aktivierung des nächsten Pumpenmodus..\n"

// oiler
#define MSG_DBG_NEW_PUMP_MODE             "Neuer Pumpemmodus ist >%s< - nächster Pumpvorgang in (Meter): %s\n" // pump mode, meter as string
#define MSG_DBG_PUMP_DEACTIVATED          "(Pumpe deaktiviert)"
#define MSG_DBG_OIL_TANK_EMPTY            "Tank leer? - Öl nachfüllen und Zähler zurücksetzen (s. Konfiguration)\n"
#define MSG_DBG_TRIGGER_PUMP              "pumpen nach (s): %d, Pumpenmodus ist >%s<\n"
#define MSG_DBG_ERROR_OPEN_OILCNT_FILE    "Fehler beim Öffnen der OilCounter Datei\n"

// update
#define MSG_DBG_UPDATE_INIT               "suche Firmware Datei '%s'\n" // firmware file
#define MSG_DBG_UPDATE_START              "Firmware gefunden - starte Update..\n"
#define MSG_DBG_UPDATE_ERROR              "FEHLER in Update.begin()\n"
#define MSG_DBG_UPDATE_SUCCESS            "Update erfolgreich eingespielt\n"
#define MSG_DBG_UPDATE_GETERROR           "Update.getError():%d\n" // Update.getError(), uint_8t, s. defines in https://github.com/espressif/arduino-esp32/blob/master/libraries/Update/src/Update.h
#define MSG_DBG_REMOVING_FIRMWARE_FILE    "lösche Firmware Datei '%s'\n" // name of firmware file
#define MSG_DBG_ERROR_DEL_FIRMWARE_FILE   "FEHLER: löschen der Firmware Datei fehlgeschlagen!!\n"
#define MSG_DBG_BOOT_AFTER_UPDATE         "Neustart nach Update..\n"
#define MSG_DBG_NO_FIRMWARE_FILE          "keine neue Firmware gefunden\n"
#define MSG_DBG_UPDATE_SERVER_URL         "URL = %s\n"
#define MSG_DBG_UPD_INFO_GET_RET_CODE     "http.GET() response: %d\n"
#define MSG_DBG_UPD_INFO_GET_RET_TEXT     "http.GET() returned: '%s'\n" // result of http.getString()
#define MSG_DBG_UPD_INFO_GET_ERROR        "http.GET() Fehler: %s\n" // %s = http.errorToString(httpCode)
#define MSG_DBG_UPD_DOWNLOAD_INFO         "downloading '%s' to '%s'\n" // source, target
#define MSG_DBG_UPD_DOWNLOAD_RET_CODE     "GET return code: %d\n"
#define MSG_DBG_UPD_DOWNLOAD_FILE_SIZE    "file size: %d bytes\n"
#define MSG_DBG_UPD_DOWNLOAD_STREAM_SIZE  "stream size available: %d\n"
#define MSG_DBG_UPD_DOWNLOAD_FILE_END     "connection closed or file end - closing file...\n"
#define MSG_DBG_UPD_DOWNLOAD_OPEN_ERROR   "Fehler beim Öffnen der Datei zum Schreiben!\n"
#define MSG_DBG_UPD_DOWNLOAD_GET_ERROR    "http.Get() Fehler: %s\n" // http.errorToString(httpCode)
#define MSG_DBG_UPD_DOWNLOAD_COMPLETE     "Download abgeschlossen: '%s'\n" // filename
#define MSG_DBG_UPD_DOWNLOAD_ERROR        "FEHLER: Download fehlgeschlagen\n"
#define MSG_DBG_RENAME_FILE_INIT          "fname = %s\n" // name of update file
#define MSG_DBG_OLD_FILE_REMOVE_ERROR     "FEHLER beim Löschen von '%s.old'\n"  // .old file
#define MSG_DBG_OLD_FILE_RENAME_ERROR     "FEHLER bei Umbenennung von '%s' in '%s.old'\n" // source, target
#define MSG_DBG_TMP_FILE_RENAME_ERROR     "FEHLER bei Umbenennung von '%s$' in '%s'\n" // source (tmp), target
#define MSG_DBG_UPD_FILE_DELETE_INIT      "lösche '%s'\n"  // name of file to delete
#define MSG_DBG_UPD_CONNECT_INIT          "suche Update Server\n"
#define MSG_HTTP_UPD_SERVER_CONNECT_ERROR "Update Server nicht erreichbar!\n"  // returned to web client
#define MSG_DBG_GET_UPDATE_INFO           "lese Update Infomationen vom Server '%s'\n" // server name
#define MSG_HTTP_NO_UPDATE_AVAILABLE      "Server meldet: kein Update vorhanden!"   // returned to web client

// upload
#define MSG_DBG_CHECK_URL_START           "check URL (GET):%s\n"  // complete url (server, port and url)
#define MSG_DBG_CHECK_URL_SERVER_IP       "IP-Adresse von '%s' ist %s\n"  // server, ip
#define MSG_DBG_CHECK_URL_ERROR           "GET() fehlgeschlagen, error: %s\n" // error message from get()
#define MSG_DBG_CHECK_URL_HTTP_ERR_401    "401: fehlende Authorisierung (Basic Authentication nötig?)\n"
#define MSG_DBG_FILECHECK_RESULT          "httpCode = %d\n" // printf: http-code returned
#define MSG_DBG_SEND_FILE_START           "Starte Übertragung der Datei...\n"
#define MSG_DBG_SEND_FILE_COMPLETED       "Übertragung beendet\n"
#define MSG_DBG_SEND_FILE_CONNECT_ERROR   "Fehler beim Öffnen der Verbindung zu: '%s:%d'\n"  // host name, port
#define MSG_DBG_SEND_FILE_FOPEN_ERROR     "Fehler beim Öffnen der Datei: %s\n"   // file name
#define MSG_DBG_TRACK_UPLOAD_START        "searching track files:\n"
#define MSG_DBG_TRACK_FILE_FOUND_YES      "'%s' is a track file - will upload...\n"
#define MSG_DBG_TRACK_FILE_FOUND_NO       "'%s' is NO track file...\n"   // file name
#define MSG_DBG_UPLOAD_SERVER_ERROR       "Upload Server nicht erreichbar! (s. Log)\n"
#define MSG_DBG_TRACK_UPLOAD_OK           "Hochladen erfolgreich.\n"
#define MSG_DBG_TRACK_UPLOAD_FAILED       "Hochladen fehlgeschlagen.\n"
#define MSG_HTTP_NO_TRACKS_FOUND          "\nKeine Dateien zum Hochladen gefunden\n"

// webhandler
#define MSG_DBG_PATH_REQUESTED            "called with path '%s'\n"
#define MSG_DBG_WEB_FILE_READ_ERROR       "FEHLER beim Lesen der Datei '%s'\n"  // file name
#define MSG_HTTP_WEB_FILE_NOT_FOUND       "FEHLER: Datei oder Link nicht vorhanden: "   // followed by link/file name
#define MSG_DBG_LINK_NOT_FOUND_CAP        "zeige index.htm, da Link/Datei nicht gefunden: '%s'\n"  // link
#define MSG_DBG_LINK_NOT_FOUND            "FEHLER: Link/Datei nicht gefunden: '%s'\n"   // link
#define MSG_DBG_LINK_SERVED               "zeige '%s'\n"                        // link
#define MSG_DBG_ERROR_INDEX_HTM_NOT_FOUND "FEHLER: konnte index.htm nicht finden!!\n"

// configuration
#define MSG_DBG_MISSING_CONF_OPTION       "[%s] Option nicht in ini Datei: %s\n"   // _FUNCTION_, name of option
