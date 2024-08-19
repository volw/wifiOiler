/************************************************************************
 * wifiOiler, an automatic distance depending motorbike chain oiler
 * Copyright (C) 2019-2024, volw
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
#define MSG_CHECK_LOG_SIZE_DELETE         "Log file exceeded maximum size and was deleted\n"
#define MSG_PROGRESS_STARTING_INIT        "********** Starting...\n"
#define MSG_PROGRESS_STARTING_END         "---------- Initialization completed.\n"
#define MSG_VERSION_MESSAGE               "This is wifiOiler version %s on board %s\n"                 // followed by Version & board type
#define MSG_ATT_PUMP_DEACTIVATED          "WARNING: Pump deactivated by define!!\n"

// FSBrowser
#define MSG_DBG_FS_FREE_SPACE_INFO        "LittleFS free: %d bytes = %d GPS records (%dh:%02d)\n" // freespace, gpsrec, gpsmin/60, gpsmin%60
#define MSG_DBG_RECORDING_STOPPED         "track recording stopped\n"
#define MSG_DBG_RECORDING_CONTINUED       "track recording continued\n"
#define MSG_HTML_TARGET_FILE_EXISTS       "Target file exists\n"
#define MSG_HTML_SOURCE_FILE_NOT_EXISTS   "Source file does'nt exist\n"
#define MSG_HTML_ERROR_OPEN_SOURCE        "ERROR: could not open source file"
#define MSG_HTML_ERROR_OPEN_TARGET        "ERROR: could not open target file"
#define MSG_HTML_ERROR_RENAMING_FILE      "Could'nt rename file"
#define MSG_HTML_ERROR_CREATING_FILE      "Could'nt create file"
#define MSG_DBG_HANDLEFILEDELETE_PATH     "%s\n"       // followed by path/name of file to delete
#define MSG_DBG_HANDLEFILEUPLOAD_URI      "GVwebServer.uri() != /edit\n"
#define MSG_DBG_HANDLEFILEUPLOAD_NAME     "Name: %s\n" // followed by (upload) filename
#define MSG_DBG_HANDLEFILEUPLOAD_SIZE     "Size: %d\n" // followed by total (upload) file size

// GPS
#define MSG_DBG_SETUP_GPS_INIT            "Starting GPS module...\n"
#define MSG_DBG_GPS_SWITCH_OFF_PUMP       "Continuous pumping is switched off...\n"
#define MSG_DBG_GPS_DATE_TIME             "date/time: %s\n"     // followed by recently created track file name
#define MSG_DBG_MOVEMENT_MOTO_MOVING      "Info: Vehicle starts to move\n"
#define MSG_DBG_MOVEMENT_MOTO_STOPPED     "Info: Vehicle STOPPED!\n"

// WiFi
#define MSG_DBG_ADD_WIFI_TO_MULTIWIFI     "Add wifi to wifiMulti: %s\n" // SSID from file "/wifi.ini"
#define MSG_DBG_START_SEARCHING_WIFI      "looking for WLAN..\n"
#define MSG_DBG_TRY_CONNECT_WIFI          "try to connect to configured WLAN..\n"
#define MSG_DBG_WIFI_NOT_CONNECTED        "not connected to any WiFi\n"
#define MSG_DBG_NO_WIFI_CONFIGS_FOUND     "no WiFi configuration(s) found in 'wifi.ini'\n"
#define MSG_DBG_CONNECT_SUCCESS           "connected to '%s', IP:%s\n"  // printf(), SSID, local IP
#define MSG_DBG_START_ACCCESS_POINT       "Setting up access point '%s' - password '%s'\n"	// printf()
#define MSG_DBG_ACCESS_POINT_IP           "My IP address: \n" // followed by Access Point IP
#define MSG_DBG_ACCESS_POINT_FAILED       "Failed setting up access point!!\n"
#define MSG_DBG_MDNS_ERROR                "Error setting up mDNS responder!...\n"
#define MSG_DBG_MDNS_STARTED              "mDNS responder started, please connect to 'http://%s.local'\n"
#define MSG_DBG_STARTING_WIFI             "starting WiFi ...\n"
#define MSG_DBG_STOPPING_WIFI             "stopping WiFi...\n"

// button
#define MSG_DBG_BUTTON_STATE              "Button state == %s\n"
#define MSG_DBG_BUTTON_PRESSED_ON_START   "Button pressed during start..\n"
#define MSG_DBG_ENTER_MAINTENANCE_MODE    "enter maintenance mode..\n"
#define MSG_DBG_BUTTON_LONG_PRESSED       "toggle WiFi requested..\n"
#define MSG_DBG_BUTTON_SHORT_PRESSED      "next pump mode requested..\n"

// oiler
#define MSG_DBG_NEW_PUMP_MODE             "New pump mode is '%s' - meters til pump: %s\n" // pump mode, meter as string
#define MSG_DBG_PUMP_DEACTIVATED          "(Pump deactivated)"
#define MSG_DBG_OIL_TANK_EMPTY            "Oil tank empty? Refill oil tank and reset counter (see configuration)\n"
#define MSG_DBG_TRIGGER_PUMP              "trigger pump after %d(s), pump mode is '%s'\n"
#define MSG_DBG_ERROR_OPEN_OILCNT_FILE    "Error opening the OilCounter file\n"

// update
#define MSG_DBG_UPDATE_INIT               "Looking for firmware file '%s'\n" // firmware file
#define MSG_DBG_UPDATE_START              "Firmware file found - updating now...\n"
#define MSG_DBG_UPDATE_ERROR              "ERROR in Update.begin()\n"
#define MSG_DBG_UPDATE_SUCCESS            "Update successfully finished\n"
#define MSG_DBG_UPDATE_GETERROR           "Update.getError():%d\n" // Update.getError(), uint_8t, s. defines in https://github.com/espressif/arduino-esp32/blob/master/libraries/Update/src/Update.h
#define MSG_DBG_REMOVING_FIRMWARE_FILE    "removing firmware file '%s'\n" // name of firmware file
#define MSG_DBG_ERROR_DEL_FIRMWARE_FILE   "ERROR removing firmware file!!\n"
#define MSG_DBG_BOOT_AFTER_UPDATE         "rebooting after update...\n"
#define MSG_DBG_NO_FIRMWARE_FILE          "no new firmware found\n"
#define MSG_DBG_UPDATE_SERVER_URL         "URL = %s\n"
#define MSG_DBG_UPD_INFO_GET_RET_CODE     "http.GET() response: %d\n"
#define MSG_DBG_UPD_INFO_GET_RET_TEXT     "http.GET() returned: '%s'\n" // result of http.getString()
#define MSG_DBG_UPD_INFO_GET_ERROR        "http.GET() failed with error: %s\n" // %s = http.errorToString(httpCode)
#define MSG_DBG_UPD_DOWNLOAD_INFO         "downloading '%s' to '%s'\n" // source, target
#define MSG_DBG_UPD_DOWNLOAD_RET_CODE     "GET return code: %d\n"
#define MSG_DBG_UPD_DOWNLOAD_FILE_SIZE    "file size: %d bytes\n"
#define MSG_DBG_UPD_DOWNLOAD_STREAM_SIZE  "stream size available: %d\n"
#define MSG_DBG_UPD_DOWNLOAD_FILE_END     "connection closed or file end - closing file...\n"
#define MSG_DBG_UPD_DOWNLOAD_OPEN_ERROR   "Error opening file for writing!\n"
#define MSG_DBG_UPD_DOWNLOAD_GET_ERROR    "http.Get() failed, error: %s\n" // http.errorToString(httpCode)
#define MSG_DBG_UPD_DOWNLOAD_COMPLETE     "download complete:: '%s'\n" // filename
#define MSG_DBG_UPD_DOWNLOAD_ERROR        "error downloading file\n"
#define MSG_DBG_RENAME_FILE_INIT          "fname = %s\n" // name of update file
#define MSG_DBG_OLD_FILE_REMOVE_ERROR     "ERROR removing '%s.old'\n"  // .old file
#define MSG_DBG_OLD_FILE_RENAME_ERROR     "ERROR renaming file '%s' to '%s.old'\n" // source, target
#define MSG_DBG_TMP_FILE_RENAME_ERROR     "ERROR renaming file '%s$' to '%s'\n" // source (tmp), target
#define MSG_DBG_UPD_FILE_DELETE_INIT      "delete '%s'\n"  // name of file to delete
#define MSG_DBG_UPD_CONNECT_INIT          "trying to connect to update server\n"
#define MSG_HTTP_UPD_SERVER_CONNECT_ERROR "Can't connect to update server"      // returned to web client
#define MSG_DBG_GET_UPDATE_INFO           "trying to retrieve update info from '%s'\n" // server name
#define MSG_HTTP_NO_UPDATE_AVAILABLE      "Server response: no update available!"   // returned to web client

// upload
#define MSG_DBG_CHECK_URL_START           "check URL (GET):%s\n"  // complete url (server, port and url)
#define MSG_DBG_CHECK_URL_SERVER_IP       "ip address of '%s' is %s\n"  // server, ip
#define MSG_DBG_CHECK_URL_ERROR           "GET() failed, error: %s\n" // error message from get()
#define MSG_DBG_CHECK_URL_HTTP_ERR_401    "401: not authorized (missing basic authentication?)\n"
#define MSG_DBG_FILECHECK_RESULT          "httpCode = %d\n" // http-code returned
#define MSG_DBG_SEND_FILE_START           "Initiate transfer of file...\n"
#define MSG_DBG_SEND_FILE_COMPLETED       "transfer completed\n"
#define MSG_DBG_SEND_FILE_CONNECT_ERROR   "error connecting to: '%s:%d'\n"  // host name, port
#define MSG_DBG_SEND_FILE_FOPEN_ERROR     "error opening file: %s\n" 	// file name
#define MSG_DBG_TRACK_UPLOAD_START        "searching track files:\n"
#define MSG_DBG_TRACK_FILE_FOUND_YES      "%s is a track file - will upload...\n"
#define MSG_DBG_TRACK_FILE_FOUND_NO       "%s is NO track file...\n"
#define MSG_DBG_UPLOAD_SERVER_ERROR       "Upload server not available! (s. Log)\n"
#define MSG_DBG_TRACK_UPLOAD_OK           "upload successful.\n"
#define MSG_DBG_TRACK_UPLOAD_FAILED       "upload FAILED.\n"
#define MSG_HTTP_NO_TRACKS_FOUND          "\nNo track files found - nothing to upload\n"

// webhandler
#define MSG_DBG_PATH_REQUESTED            "called with path '%s'\n"
#define MSG_DBG_WEB_FILE_READ_ERROR       "ERROR reading file '%s'\n"  // file name
#define MSG_HTTP_WEB_FILE_NOT_FOUND       "ERROR: file or link not found: "              // followed by link/file name
#define MSG_DBG_LINK_NOT_FOUND_CAP        "serve index.htm, not found: '%s'\n"    // link
#define MSG_DBG_LINK_NOT_FOUND            "ERROR: file or link not found: '%s'\n" // link
#define MSG_DBG_LINK_SERVED               "serve '%s'\n"                          // link
#define MSG_DBG_ERROR_INDEX_HTM_NOT_FOUND "ERROR: index.htm not found!!\n"

// configuration
#define MSG_DBG_MISSING_CONF_OPTION       "[%s] missing parameter in ini file: %s\n"   // _FUNCTION_, name of option
