// häufiger verwendete Token (spart ein wenig Speicherplatz):
const char PROGMEM * TEXT_PLAIN = "text/plain";
const char PROGMEM * TEXT_HTML = "text/html";
const char PROGMEM * BAD_PATH = "BAD PATH";
const char PROGMEM * BAD_ARGS = "BAD ARGS";

/**********************************************************************
 * if not enough space for track recording (less than MIN_MINUTES_FREE)
 * stop recording
 **********************************************************************/
void checkFilesystemSpace(void)
{
  if (currentfpw > 0) {  // only check, if recording still active
    // wenn nur noch Platz für weniger als MIN_MINUTES_FREE, dann tracking deaktivieren
    FSInfo fs_info;
    if (_FILESYS.info(fs_info)) {
      uint32_t freespace = fs_info.totalBytes - (uint32_t)(fs_info.usedBytes * 1.05);
      uint32_t gpsrec = freespace / GPS_RECORD_SIZE;    // Platz für (gpsrec) GPS Records
      uint32_t gpsmin = (gpsrec * conf.fpw) / 60;     // Umrechnung in Minuten
      uint16_t gpshours = gpsmin / 60;
  
      // diese Abfrage unbedingt VOR gpsmin "Reduzierung"
      if (conf.fpw > 0) {
        currentfpw = gpsmin < MIN_MINUTES_FREE ? 0 : conf.fpw;
      }
      else currentfpw = 0;  // to be sure... (Aufzeichnung deaktiviert)
      
      gpsmin = gpsmin % 60;
  
      String out = String(gpsmin);
      if (out.length() < 2) out = '0' + out;
      out = "LittleFS free: " + String(freespace) + " bytes = " + String(gpsrec) + " GPS records (" + String(gpshours) + ":" + out + ")";
      
      DEBUG_OUT.println(out);
  
      lastFScheck = millis();
    }
  } //else DEBUG_OUT.println(F("[checkFilesystemSpace] no check / tracking allready inactive"));
}

/****************************************************
 * webHandler für FileBrowser: Dateiliste als JSON
 ****************************************************/
void handleFileList() {
  Dir dir;
  if (webServer.hasArg("dir"))
    dir = _FILESYS.openDir("/");
  else if (webServer.hasArg("tracks"))
    dir = _FILESYS.openDir("/2");
  else {
    webServer.send(500, TEXT_PLAIN, BAD_ARGS);
    return;
  }

  // Beispiel Ausgabe:  [{"type":"file","name":"config.htm"},{"type":"file","name":"connecttest.txt"}]
  String output = "[";
  // Dir dir = _FILESYS.openDir("/");
  while (dir.next()) {
    if (output != "[") output += ',';
    #ifdef _SPIFFS_
      //im SPIFFS FS wird Dateiname mit leitendem "/" zurückgegeben (dir.filename())
      //im LittleFS NICHT!
      output += "{\"type\":\"file\",\"name\":\""+dir.fileName().substring(1)+"\"}";
    #else      
      //LittleFS:
      output += "{\"type\":\"file\",\"name\":\""+dir.fileName()+"\"}";
    #endif
  }
  output += "]";
  webServer.send(200, F("text/json"), output);
}

/**********************************************
 * webHandler für FileBrowser: Datei anlegen
 *                             Datei umbenennen
 **********************************************/
void handleFileCreate() {
  if (webServer.args() == 0) {
    return webServer.send(500, TEXT_PLAIN, BAD_ARGS);
  }
  String newPath = webServer.arg(0);
    
  DEBUG_OUT.println("handleFileCreate: " + newPath);
  if (newPath == "/") {
    return webServer.send(500, TEXT_PLAIN, BAD_PATH);
  }
  if (_FILESYS.exists(newPath)) {
    return webServer.send(500, TEXT_PLAIN, F("Target file exists"));
  }
  if (webServer.args() > 1) {   // Datei umbenennen
    String oldPath = webServer.arg(1);
    if (!_FILESYS.exists(oldPath)) {
      return webServer.send(500, TEXT_PLAIN, F("Source file does'nt exist"));
    }
    if (!_FILESYS.rename(oldPath, newPath)) { // old, new
      return webServer.send(500, TEXT_PLAIN, F("Could'nt rename file"));
    }
  }
  else  // neue Datei anlegen
  {
    File file = _FILESYS.open(newPath, "w");
    if (file) {
      file.close();
    } else {
      return webServer.send(500, TEXT_PLAIN, F("Could'nt create file"));
    }
  }
  webServer.send(200, TEXT_PLAIN, "");
}

/***************************************************
 * webHandler für FileBrowser: Datei löschen
 ***************************************************/
void handleFileDelete() {
  if (webServer.args() == 0) {
    return webServer.send(500, TEXT_PLAIN, BAD_ARGS);
  }
  String path = webServer.arg(0);
  DEBUG_OUT.println("handleFileDelete: " + path);
  if (path == "/") {
    return webServer.send(500, TEXT_PLAIN, BAD_PATH);
  }
  if (!_FILESYS.exists(path)) {
    return webServer.send(404, TEXT_PLAIN, F("FileNotFound"));
  }
  _FILESYS.remove(path);

  checkFilesystemSpace();
  webServer.send(200, TEXT_PLAIN, "");
  path = String();
}

/***********************************************
 * webHandler für Upload einer Datei
 * danach Check auf Firmware und Speicherplatz
 ***********************************************/
void handleFileUpload() {
  if (webServer.uri() != F("/edit")) {
    DEBUG_OUT.println(F("handleFileUpload: webServer.uri() != /edit"));
    return;
  }
  HTTPUpload& upload = webServer.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String fname = upload.filename;
    myDisplay.PrintMessage("FileUpload\n");
    myDisplay.MessageAdd(fname);
    if (!fname.startsWith("/")) {
      fname = "/" + fname;
    }
    DEBUG_OUT.print(F("handleFileUpload Name: ")); DEBUG_OUT.println(fname);
    fsUploadFile = _FILESYS.open(fname, "w");
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    myLedx.on(LED_GRUEN);
    if (fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
    myLedx.off();
  } else if (upload.status == UPLOAD_FILE_END) {
    DEBUG_OUT.print(F("handleFileUpload Size: ")); DEBUG_OUT.println(upload.totalSize);
    if (fsUploadFile) {
      fsUploadFile.close();
      myLedx.start LED_FILE_UPLOAD_SUCCESS;
      myDisplay.PrintMessage("Success");
      myLedx.delay();
      checkforUpdate();
    }
    checkFilesystemSpace();
  }
}
