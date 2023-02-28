/************************************************************************
 * wifiOiler, an automatic distance depending motorbike chain oiler
 * Copyright (C) 2019-2022, volw
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

/**********************************************************************
 * if not enough space for track recording (less than MIN_MINUTES_FREE)
 * stop recording
 **********************************************************************/
void checkFilesystemSpace(void)
{
  if (GVoilerConf.fpw > 0){  // also nur, wenn aufgezeichnet werden SOLLTE
    FSInfo fs_info;
    if (_FILESYS.info(fs_info)) {
      uint32_t freespace = fs_info.totalBytes - (uint32_t)(fs_info.usedBytes * 1.05);
      uint32_t gpsrec = freespace / GPS_RECORD_SIZE;    // Platz für (gpsrec) GPS Records
      uint32_t gpsmin = ((gpsrec * GVoilerConf.fpw) / 60) - MIN_MINUTES_FREE;     // Umrechnung in Gesamt-Minuten

      DEBUG_OUT.printf(PSTR(MSG_DBG_FS_FREE_SPACE_INFO), freespace, gpsrec, gpsmin/60, gpsmin%60);
      
      if (GVcurrentfpw > 0 && gpsmin < MIN_MINUTES_FREE) {  // Aufzeichnung aktiv aber zu wenig Platz?
        GVcurrentfpw = 0;                                   // Aufzeichnung deaktivieren
        DEBUG_OUT.println(F(MSG_DBG_RECORDING_STOPPED));
      }
      else if (GVcurrentfpw <= 0 && gpsmin >= MIN_MINUTES_FREE) {
        GVcurrentfpw = GVoilerConf.fpw;
        DEBUG_OUT.println(F(MSG_DBG_RECORDING_CONTINUED));
      }
      GVlastFScheck = millis();
    }
  }
}

/****************************************************
 * webHandler für FileBrowser: Dateiliste als JSON
 ****************************************************/
void handleFileList() {
  Dir dir;
  if (GVwebServer.hasArg("dir"))
    dir = _FILESYS.openDir("/");
  else if (GVwebServer.hasArg("tracks"))
    dir = _FILESYS.openDir("/"+GVoilerConf.gts);
  else {
    GVwebServer.send(500, TEXT_PLAIN, BAD_ARGS);
    return;
  }

  // Beispiel Ausgabe:  [{"type":"file","name":"config.htm"},{"type":"file","name":"connecttest.txt"}]
  String output = "[";

  while (dir.next()) {
    if (output != "[") output += ',';
    //im SPIFFS FS wird Dateiname mit leitendem "/" zurückgegeben (dir.filename())
    //im LittleFS NICHT!
    output += "{\"type\":\"file\",\"name\":\""+dir.fileName()+"\",\"size\":"+dir.fileSize()+"}";
  }
  output += "]";
  GVwebServer.send(200, F("text/json"), output);
}

/**********************************************
 * webHandler für FileBrowser: Datei anlegen
 *                             Datei umbenennen
 **********************************************/
void handleFileCreate() {
  if (GVwebServer.args() == 0) {
    return GVwebServer.send(500, TEXT_PLAIN, BAD_ARGS);
  }
  String newPath = GVwebServer.arg(0);
    
  DEBUG_OUT.println("handleFileCreate: " + newPath);
  if (newPath == "/") {
    return GVwebServer.send(500, TEXT_PLAIN, BAD_PATH);
  }
  if (_FILESYS.exists(newPath)) {
    return GVwebServer.send(500, TEXT_PLAIN, F(MSG_HTML_TARGET_FILE_EXISTS));
  }
  if (GVwebServer.args() > 1) {   // Datei umbenennen
    String oldPath = GVwebServer.arg(1);
    if (!_FILESYS.exists(oldPath)) {
      return GVwebServer.send(500, TEXT_PLAIN, F(MSG_HTML_SOURCE_FILE_NOT_EXISTS));
    }
    if (!_FILESYS.rename(oldPath, newPath)) { // old, new
      return GVwebServer.send(500, TEXT_PLAIN, F(MSG_HTML_ERROR_RENAMING_FILE));
    }
  }
  else  // neue Datei anlegen
  {
    File file = _FILESYS.open(newPath, "w");
    if (file) {
      file.close();
    } else {
      return GVwebServer.send(500, TEXT_PLAIN, F(MSG_HTML_ERROR_CREATING_FILE));
    }
  }
  GVwebServer.send(200, TEXT_PLAIN, "");
}

/***************************************************
 * webHandler für FileBrowser: Datei löschen
 ***************************************************/
void handleFileDelete() {
  if (GVwebServer.args() == 0) {
    return GVwebServer.send(500, TEXT_PLAIN, BAD_ARGS);
  }
  String path = GVwebServer.arg(0);
  DEBUG_OUT.print(F(MSG_DBG_HANDLEFILEDELETE_PATH));
  DEBUG_OUT.println(path);
  if (path == "/") {
    return GVwebServer.send(500, TEXT_PLAIN, BAD_PATH);
  }
  if (!_FILESYS.exists(path)) {
    return GVwebServer.send(404, TEXT_PLAIN, F("FileNotFound"));
  }
  _FILESYS.remove(path);

  checkFilesystemSpace();
  GVwebServer.send(200, TEXT_PLAIN, "OK");
  path = String();
}

/***********************************************
 * webHandler für Upload einer Datei
 * danach Check auf Firmware und Speicherplatz
 ***********************************************/
void handleFileUpload() {
  if (GVwebServer.uri() != F("/edit")) {
    DEBUG_OUT.println(F(MSG_DBG_HANDLEFILEUPLOAD_URI));
    return;
  }
  HTTPUpload& upload = GVwebServer.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String fname = upload.filename;
    GVmyDisplay.PrintMessage("FileUpload\n");
    GVmyDisplay.MessageAdd(fname);
    if (!fname.startsWith("/")) {
      fname = "/" + fname;
    }
    DEBUG_OUT.print(F(MSG_DBG_HANDLEFILEUPLOAD_NAME)); DEBUG_OUT.println(fname);
    GVfsUploadFile = _FILESYS.open(fname, "w");
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    GVmyLedx.on(LED_GRUEN);
    if (GVfsUploadFile) {
      GVfsUploadFile.write(upload.buf, upload.currentSize);
    }
    GVmyLedx.off();
  } else if (upload.status == UPLOAD_FILE_END) {
    DEBUG_OUT.print(F(MSG_DBG_HANDLEFILEUPLOAD_SIZE)); DEBUG_OUT.println(upload.totalSize);
    if (GVfsUploadFile) {
      GVfsUploadFile.close();
      GVmyLedx.start LED_FILE_UPLOAD_SUCCESS;
      GVmyDisplay.PrintMessage("Success");
      GVmyLedx.delay();
      checkforUpdate();
    }
    checkFilesystemSpace();
  }
}
