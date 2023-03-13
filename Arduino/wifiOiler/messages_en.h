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
#define MSG_GLOBAL_YES "Yes"
#define MSG_GLOBAL_NO  "No"

// wifiOiler.ino
#define MSG_ERROR_INITIALISING_FILESYSTEM "Error initialising LittleFS file system!"
#define MSG_ERROR_INITIALISING_LOGGER     "Error initialising LittleFS Logger!"
#define MSG_CHECK_LOG_SIZE_DELETE         "[setup] Log file exceeded maximum size and was deleted"
#define MSG_PROGRESS_STARTING_INIT        "********** Starting..."
#define MSG_PROGRESS_STARTING_END         "---------- Initialization completed."
#define MSG_VERSION_MESSAGE_1             "This is wifiOiler version " // followed by version + MSG_VERSION_MESSAGE_2
#define MSG_VERSION_MESSAGE_2             " on board "                 // followed by board type
#define MSG_ATT_PUMP_DEACTIVATED          "WARNING: Pump deactivated by define!!"

// FSBrowser
#define MSG_DBG_FS_FREE_SPACE_INFO        "[checkFilesystemSpace] LittleFS free: %d bytes = %d GPS records (%dh:%02d)\n" // freespace, gpsrec, gpsmin/60, gpsmin%60
#define MSG_DBG_RECORDING_STOPPED         "[checkFilesystemSpace] track recording stopped"
#define MSG_DBG_RECORDING_CONTINUED       "[checkFilesystemSpace] track recording continued"
#define MSG_HTML_TARGET_FILE_EXISTS       "Target file exists"
#define MSG_HTML_SOURCE_FILE_NOT_EXISTS   "Source file does'nt exist"
#define MSG_HTML_ERROR_RENAMING_FILE      "Could'nt rename file"
#define MSG_HTML_ERROR_CREATING_FILE      "Could'nt create file"
#define MSG_DBG_HANDLEFILEDELETE_PATH     "[handleFileDelete] "       // followed by path/name of file to delete
#define MSG_DBG_HANDLEFILEUPLOAD_URI      "[handleFileUpload] GVwebServer.uri() != /edit"
#define MSG_DBG_HANDLEFILEUPLOAD_NAME     "[handleFileUpload] Name: " // followed by (upload) filename
#define MSG_DBG_HANDLEFILEUPLOAD_SIZE     "[handleFileUpload] Size: " // followed by total (upload) file size

// GPS
#define MSG_DBG_SETUP_GPS_INIT            "[setupGPS] Starting GPS module..."
#define MSG_DBG_GPS_SWITCH_OFF_PUMP       "[checkGPSdata] Continuous pumping is switched off..."
#define MSG_DBG_GPS_DATE_TIME             "[createDateFilename] date/time: " // followed by recently created track file name
#define MSG_DBG_MOVEMENT_MOTO_MOVING      "[checkMovement] Info: Vehicle starts to move"
#define MSG_DBG_MOVEMENT_MOTO_STOPPED     "[checkMovement] Info: Vehicle STOPPED!"

// WiFi
#define MSG_DBG_ADD_WIFI_TO_MULTIWIFI     "[readWifiData] Add wifi to wifiMulti: " // followed by SSID from file "/wifi.ini"
#define MSG_DBG_START_SEARCHING_WIFI      "[setupWiFi] looking for WLAN.."
#define MSG_DBG_TRY_CONNECT_WIFI          "[setupWiFi] try to connect to configured WLAN "
#define MSG_DBG_WIFI_NOT_CONNECTED        "[setupWiFi] not connected to any WiFi"
#define MSG_DBG_NO_WIFI_CONFIGS_FOUND     "[setupWiFi] no WiFi configuration(s) found in 'wifi.ini'"
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

// oiler
#define MSG_DBG_NEW_PUMP_MODE             "New pump mode is >%s< - meters til pump: " // printf: no eol, followed by meters
#define MSG_DBG_PUMP_DEACTIVATED          "(Pumpe deactivated)"
#define MSG_DBG_OIL_TANK_EMPTY            "[checkTank] Tank empty? Refill oil tank and reset counter (see configuration)"
#define MSG_DBG_TRIGGER_PUMP              "[TriggerPump] trigger pump after %d(s), pump mode is >%s<\n"
#define MSG_DBG_ERROR_OPEN_OILCNT_FILE    "[TriggerPump] Error opening the OilCounter file"

// update
#define MSG_DBG_FILE_CHECK_REMOVE_ERROR   "[checkOldFiles] ERROR deleting old update file: "
#define MSG_DBG_FILE_CHECK_REMOVE_SUCCESS "[checkOldFiles] old update file deleted: "
#define MSG_DBG_UPDATE_INIT               "[checkforUpdate] Looking for firmware file " // followed by name of firmware file
#define MSG_DBG_UPDATE_START              "[checkforUpdate] Firmware file found - updating now..."
#define MSG_DBG_UPDATE_ERROR              "[checkforUpdate] ERROR"
#define MSG_DBG_UPDATE_SUCCESS            "[checkforUpdate] Update successfully finished"
#define MSG_DBG_UPDATE_GETERROR           "[checkforUpdate] Update.getError():" // followed by Update.getError()
#define MSG_DBG_REMOVING_FIRMWARE_FILE    "[checkforUpdate] removing firmware file " // followed by name of firmware file
#define MSG_DBG_ERROR_DEL_FIRMWARE_FILE   "[checkforUpdate] ERROR removing firmware file!!"
#define MSG_DBG_BOOT_AFTER_UPDATE         "[checkforUpdate] rebooting after update..."
#define MSG_DBG_NO_FIRMWARE_FILE          "[checkforUpdate] no new firmware found"
#define MSG_DBG_UPDATE_SERVER_URL         "[getUpdateInfo] URL = %s\n"	// printf
#define MSG_DBG_UPD_INFO_GET_RET_CODE     "[getUpdateInfo] http.GET() response: %d\n" // printf
#define MSG_DBG_UPD_INFO_GET_RET_TEXT     "[getUpdateInfo] http.GET() returned: " // followed by result of http.getString()
#define MSG_DBG_UPD_INFO_GET_ERROR        "[getUpdateInfo] http.GET() failed, error: %s\n" // printf, %s = http.errorToString(httpCode)
#define MSG_DBG_UPD_DOWNLOAD_INFO         "[downloadFile] downloading '%s' to '%s'\n" // printf, source, target
#define MSG_DBG_UPD_DOWNLOAD_RET_CODE     "[downloadFile] GET return code: %d\n" // printf
#define MSG_DBG_UPD_DOWNLOAD_OPEN_ERROR   "[downloadFile] Error opening file for writing!"
#define MSG_DBG_UPD_DOWNLOAD_GET_ERROR    "[downloadFile] http.Get() failed, error: %s\n" // printf, http.errorToString(httpCode)
#define MSG_DBG_UPD_DOWNLOAD_COMPLETE     "[handleUpdate] download complete:" // followed by filename
#define MSG_DBG_UPD_DOWNLOAD_ERROR        "[handleUpdate] error downloading file"
#define MSG_DBG_RENAME_FILE_INIT          "[renameUpdateFiles] fname = " // followed by name of update file
#define MSG_DBG_OLD_FILE_REMOVE_ERROR     "[renameUpdateFiles] ERROR removing "  // followed by .old file
#define MSG_DBG_OLD_FILE_RENAME_ERROR     "[renameUpdateFiles] ERROR renaming file '%s' to '%s.old'\n" // printf, source, target
#define MSG_DBG_TMP_FILE_RENAME_ERROR     "[renameUpdateFiles] ERROR renaming file '%s$' to '%s'\n" // printf, source (tmp), target
#define MSG_DBG_UPD_FILE_DELETE_INIT      "[deleteUpdateFiles] delete "  // followed by name of file to delete
#define MSG_DBG_UPD_CONNECT_INIT          "[handleUpdate] trying to connect to update server\n"
#define MSG_HTTP_UPD_SERVER_CONNECT_ERROR ("Update Server '"+GVoilerConf.uhn+":"+GVoilerConf.uhp+"' nicht erreichbar!")
#define MSG_DBG_GET_UPDATE_INFO           "[handleUpdate] trying to retrieve update info from " // followed by server name
#define MSG_HTTP_NO_UPDATE_AVAILABLE      "Server response: no update available!"

// upload
#define MSG_DBG_CHECK_URL_START           "[isServerAvailable] check URL (GET):%s\n"  // printf: complete url (server, port and url)
#define MSG_DBG_CHECK_URL_SERVER_IP       "[isServerAvailable] ip address of '%s' is %s\n"  // printf: server, ip
#define MSG_DBG_CHECK_URL_ERROR           "[isServerAvailable] GET() failed, error: %s\n" // printf: error message from get()
#define MSG_DBG_CHECK_URL_HTTP_ERR_401    "[isServerAvailable] 401: not authorized (missing basic authentication?)"
#define MSG_DBG_FILECHECK_RESULT          "[isFileThere] httpCode = %d\n" // printf: http-code returned
#define MSG_DBG_SEND_FILE_START           "[sendFile] Initiate transfer of file..."
//#define MSG_DBG_SEND_FILE_CONTENT         "[sendFile] >>> sending file content >>>"
#define MSG_DBG_SEND_FILE_COMPLETED       "[sendFile] transfer completed"
#define MSG_DBG_SEND_FILE_CONNECT_ERROR   "[sendFile] error connecting to: '%s:%d'\n"  // printf: host name, port
#define MSG_DBG_SEND_FILE_FOPEN_ERROR     "[sendFile] error opening file: %s\n" 	// printf: file name
#define MSG_DBG_TRACK_UPLOAD_START        "[handleUpload] searching track files:"
#define MSG_DBG_TRACK_FILE_FOUND_YES      "[handleUpload] %s is a track file - will upload..."  // followed by OK or FAILED (so no line feed)
#define MSG_DBG_TRACK_FILE_FOUND_NO       "[handleUpload] %s is NO track file...\n"
#define MSG_DBG_UPLOAD_SERVER_ERROR       "[handleUpload] Upload server not available! (s. Log)"
#define MSG_DBG_TRACK_UPLOAD_OK           "[handleUpload] upload successful."
#define MSG_DBG_TRACK_UPLOAD_FAILED       "[handleUpload] upload FAILED."
#define MSG_HTTP_NO_TRACKS_FOUND          "\nNo track files found - nothing to upload\n-END-"

// webhandler
#define MSG_DBG_WEB_FILE_READ_ERROR       "[handleFileRead] error reading file " // followed by file name (path)

// configuration
#define MSG_DBG_MISSING_CONF_OPTION       "[fworker] missing parameter in ini file: %s\n"   // printf (name of option)
