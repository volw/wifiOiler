/****
 *      wifiOiler, an automatic distance depending motorbike chain oiler
 *      Copyright (C) 2019-2021, volw
 *
 *      This program is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program.  If not, see <https://www.gnu.org/licenses/>.
 ****/

// wifiOiler.ino
#define MSG_ERROR_INITIALISING_FILESYSTEM "Fehler bei Initialisierung des Dateisystems!"
#define MSG_ERROR_INITIALISING_LOGGER     "Fehler bei Initialisierung des (LittleFS) Logger!"
#define MSG_PROGRESS_STARTING_INIT        "********** Starting..."
#define MSG_PROGRESS_STARTING_END         "-------- Initialisierung beendet."
#define MSG_VERSION_MESSAGE_1             "This is wifiOiler version " // followed by version + MSG_VERSION_MESSAGE_2
#define MSG_VERSION_MESSAGE_2             " on board "                 // followed by board type
#define MSG_ATT_PUMP_DEACTIVATED          "ACHTUNG: Pumpe dauerhaft deaktiviert!!"

// FSBrowser.h
#define MSG_HTML_TARGET_FILE_EXISTS       "Target file exists"
#define MSG_HTML_SOURCE_FILE_NOT_EXISTS   "Source file does'nt exist"
#define MSG_HTML_ERROR_RENAMING_FILE      "Could'nt rename file"
#define MSG_HTML_ERROR_CREATING_FILE      "Could'nt create file"
#define MSG_DBG_HANDLEFILEDELETE_PATH     "[handleFileDelete] "       // followed by path/name of file to delete
#define MSG_DBG_HANDLEFILEUPLOAD_URI      "[handleFileUpload] GVwebServer.uri() != /edit"
#define MSG_DBG_HANDLEFILEUPLOAD_NAME     "[handleFileUpload] Name: " // followed by (upload) filename
#define MSG_DBG_HANDLEFILEUPLOAD_SIZE     "[handleFileUpload] Size: " // followed by total (upload) file size
