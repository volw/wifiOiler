/*****************************************************************
 * setup Web Server handler
 *****************************************************************/
void setupWebServer(void) {
  //list directory
  webServer.on(F("/list"), HTTP_GET, handleFileList);
  webServer.on(F("/info"), HTTP_GET, handleDebugInfo);
  webServer.on(F("/pumptest"), HTTP_GET, handlePumpTest);
  webServer.on(F("/ledtest"), HTTP_GET, handleLEDTest);
  webServer.on(F("/config"), HTTP_GET, handleConfigPage);
  webServer.on(F("/config"), HTTP_POST, handleConfigPage);
  webServer.on(F("/upload"), HTTP_GET, handleUpload);
  webServer.on(F("/pumpmode"), HTTP_GET, handlePumpMode);
  webServer.on(F("/reboot"), HTTP_GET, handleReboot);
  webServer.on(F("/update"), HTTP_GET, handleUpdate);
  webServer.on(F("/version"), HTTP_GET, handleVersion);
 
  //LittleFS Browser & editor:
  webServer.on(F("/edit"), HTTP_GET, []() {
    if (!handleFileRead(conf.fbe)) {
      webServer.send(404, TEXT_PLAIN, F("FileNotFound"));
    }
  });
  webServer.on(F("/edit"), HTTP_PUT, handleFileCreate);     //create file
  webServer.on(F("/edit"), HTTP_DELETE, handleFileDelete);  //delete file
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  webServer.on(F("/edit"), HTTP_POST, []() {
    webServer.send(200, TEXT_PLAIN, "");
  }, handleFileUpload);

  webServer.onNotFound([]() {
    if (!handleFileRead(webServer.uri())) {
      webServer.send(404, TEXT_PLAIN, F("FileNotFound"));
    }
  });
  webServer.begin();
}

/*************************************************
 * Content type anhand Dateiendung identifizieren
 *************************************************/
String getContentType(String fname) {
  if (webServer.hasArg(F("download"))) return F("application/octet-stream");
  else if (fname.endsWith(F(".htm")))  return TEXT_HTML;
  else if (fname.endsWith(F(".html"))) return TEXT_HTML;
  else if (fname.endsWith(F(".css")))  return F("text/css");
  else if (fname.endsWith(F(".js")))   return F("application/javascript");
  else if (fname.endsWith(F(".png")))  return F("image/png");
  else if (fname.endsWith(F(".gif")))  return F("image/gif");
  else if (fname.endsWith(F(".jpg")))  return F("image/jpeg");
  else if (fname.endsWith(F(".ico")))  return F("image/x-icon");
  else if (fname.endsWith(F(".xml")))  return F("text/xml");
  else if (fname.endsWith(F(".pdf")))  return F("application/x-pdf");
  else if (fname.endsWith(F(".zip")))  return F("application/x-zip");
  else if (fname.endsWith(F(".gz")))   return F("application/x-gzip");
  else return TEXT_PLAIN;
}



/*************************************************
 * Lesen einer Datei aus LittleFS Dateisystem
 * HTML oder *.gz gezippte HTML
 * HTML wird vorgezogen, falls beide existieren
 *************************************************/
bool handleFileRead(String path) 
{
  DEBUG_OUT.println("handleFileRead: " + path);
  // wenn keine Datei mitgegeben wurde, nehmen wir index.htm an:
  if (path.endsWith("/")) path += "index.htm";

  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  
  if (_FILESYS.exists(pathWithGz) || _FILESYS.exists(path)) {
    File file;  
    if (_FILESYS.exists(path))
      file = _FILESYS.open(path, "r");
    else 
      file = _FILESYS.open(pathWithGz, "r");

    if (file) {
      webServer.streamFile(file, contentType);
      file.close();
    }
    else 
    {
      DEBUG_OUT.print(F("[handleFileRead] Fehler beim Download (open) der Datei: "));
      DEBUG_OUT.println(path);
    }
    return true;
  }
  return false;
}

/******************************************************
 * Hilfsfunktion zum Anzeigen einer Meldung und 
 * weiterer Aktion:
 * justBack == true : zurück per history.back() (default)
 * justBack == false: expliziter Aufruf des Hauptmenüs
 ******************************************************/ 
void handleMessage(String message, bool justBack) // default=true
{
  webServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  webServer.send( 200, TEXT_HTML, "");
  //webServer.sendContent(F("<!DOCTYPE html><html><head>"));
  webServer.sendContent(F("<meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\">"));
  webServer.sendContent(F("</head><body><script type=\"text/javascript\">"));
  webServer.sendContent(F("function msg(){alert(\""));
  webServer.sendContent(message);
  if (justBack)
    webServer.sendContent(F("\");history.back();}"));
  else
    webServer.sendContent(F("\");window.location.replace('/');}"));
  webServer.sendContent(F("window.onload = msg; </script></body></html>"));
  webServer.sendContent(""); // this tells web client that transfer is done
  webServer.client().stop();
}

/***************************************************
 * webHandler: Test Pumpe
 ***************************************************/
void handlePumpTest(void)
{
  InitiatePump();
  handleMessage(F("Pumpentest gestartet"));
}

/***************************************************
 * webHandler: Test LED
 ***************************************************/
void handleLEDTest(void)
{
  myLedx.add LED_TEST_GRUEN;
  myLedx.add (0, 300);
  myLedx.add LED_TEST_ROT;
  myLedx.start();
  handleMessage(F("LED Test gestartet (erst gruen, dann rot)"));
}

/***************************************************
 * Gibt die aktuelle Version als plain text zurück
 ***************************************************/
void handleVersion(void) {
  webServer.send(200, TEXT_PLAIN, conf.apn+" v"+String(VERSION));
}

/***************************************************
 * webHandler: Reboot oiler
 * serve html which initiates reboot...
 ***************************************************/
void handleReboot(void)
{
  if (webServer.hasArg(F("bootnow"))){
    myLedx.on(LED_ROT);
    myDisplay.PrintMessage("Reboot...");
    //handleMessage(F("rebooting..."));
    webServer.send( 200, TEXT_HTML, "OK" );
    webServer.handleClient();
    
    //uint32_t wait = millis();
    //while (wait + 1000 > millis()) webServer.handleClient();
    ESP.restart();
  } else handleFileRead("/reboot.htm");
}

/***************************************************
 * Interaktives Umschalten der Pump Modi
 ***************************************************/
void handlePumpMode(void) {
  if (webServer.hasArg(F("mode")))
  {
    String modestr = webServer.arg(F("mode"));
    if (modestr.equalsIgnoreCase(getPumpModeStr(MODE_NORMAL))) setNewMode(MODE_NORMAL);
    else if (modestr.equalsIgnoreCase(getPumpModeStr(MODE_REGEN))) setNewMode(MODE_REGEN);
    else if (modestr.equalsIgnoreCase(getPumpModeStr(MODE_OFFROAD))) setNewMode(MODE_OFFROAD);
    else if (modestr.equalsIgnoreCase(getPumpModeStr(MODE_PERMANENT))) setNewMode(MODE_PERMANENT);
    else if (modestr.equalsIgnoreCase(getPumpModeStr(MODE_OFF))) setNewMode(MODE_OFF);
    else
    {
      return webServer.send(200, TEXT_PLAIN, "UNKNOWN");
    }
    return webServer.send(200, TEXT_PLAIN, "OK");
  }
  else // return current pumpmode
  {
    webServer.send(200, TEXT_PLAIN, getPumpModeStr(pumpMode));
  }
}

/***********************************************************
 * Liefern aller Config Daten für die entsprechende Web-Page
 * oder
 * parsen der zurückgegebenen Config Werte und speichern
 ***********************************************************/
void handleConfigPage(void) {
  if (webServer.args() == 0)
  {
    // sending config values as json string:
    String output = "{\"wts\":\"" + String(conf.wts) + "\"";
    output += ",\"nmm\":\"" + String(conf.nmm) + "\"";
    output += ",\"rmm\":\"" + String(conf.rmm) + "\"";
    output += ",\"omm\":\"" + String(conf.omm) + "\"";
    output += ",\"sim\":\"" + String(conf.sim) + "\"";
    output += ",\"pac\":\"" + String(conf.pac) + "\"";
    output += ",\"glf\":\"" + String(conf.glf) + "\"";
    output += ",\"otk\":\"" + String(conf.otk) + "\"";
    output += ",\"use\":\"" + String(conf.use) + "\"";
    output += ",\"ffn\":\"" + String(conf.ffn) + "\"";
    output += ",\"fpw\":\"" + String(conf.fpw) + "\"";
    output += ",\"apn\":\"" + String(conf.apn) + "\"";
    output += ",\"app\":\"" + String(conf.app) + "\"";
    output += ",\"uhn\":\"" + String(conf.uhn) + "\"";
    output += ",\"uhp\":\"" + String(conf.uhp) + "\"";
    output += ",\"url\":\"" + String(conf.url) + "\"";
    output += ",\"lgf\":\"" + String(conf.lgf) + "\"";
    output += ",\"lgs\":\"" + String(conf.lgs) + "\"";
    output += ",\"gdl\":\"" + String(conf.gdl) + "\"";
    output += ",\"wso\":\"" + String(conf.wso) + "\"";
    output += ",\"gcf\":\"" + String(conf.gcf) + "\"";
    output += ",\"fbe\":\"" + String(conf.fbe) + "\"}";
    webServer.send(200, F("text/json"), output);
  } else {
    // Konfiguration aus den Argumenten lesen
    if (webServer.hasArg(F("wts"))) conf.wts = webServer.arg(F("wts")).toInt();   // Wartezeit Simulation (s)
    if (webServer.hasArg(F("nmm"))) conf.nmm = webServer.arg(F("nmm")).toInt();   // Normal Mode Meters
    if (webServer.hasArg(F("rmm"))) conf.rmm = webServer.arg(F("rmm")).toInt();   // Regen Mode Meters
    if (webServer.hasArg(F("omm"))) conf.omm = webServer.arg(F("omm")).toInt();   // Offroad Mode Meters
    if (webServer.hasArg(F("sim"))) conf.sim = webServer.arg(F("sim")).toInt();   // Simulation Meter (Notprogramm)
    if (webServer.hasArg(F("pac"))) conf.pac = webServer.arg(F("pac")).toInt();   // Pump Action Count
    if (webServer.hasArg(F("glf"))) conf.glf = webServer.arg(F("glf")).toInt();   // GPS Low-Filter
    if (webServer.hasArg(F("otk"))) conf.otk = webServer.arg(F("otk")).toInt();   // Tank Kapazität
    if (webServer.hasArg(F("use"))) conf.use = webServer.arg(F("use")).toInt();   // ..davon verbraucht
    if (webServer.hasArg(F("ffn"))) conf.ffn = webServer.arg(F("ffn"));           // Firmware FileName
    if (webServer.hasArg(F("fpw"))) conf.fpw = webServer.arg(F("fpw")).toInt();   // Abstand zwischen GPS writes (s)
    if (webServer.hasArg(F("apn"))) conf.apn = webServer.arg(F("apn"));           // Access Point Name
    if (webServer.hasArg(F("app"))) conf.app = webServer.arg(F("app"));           // Access Point Passwort
    if (webServer.hasArg(F("uhn"))) conf.uhn = webServer.arg(F("uhn"));           // Upload Host Name
    if (webServer.hasArg(F("uhp"))) conf.uhp = webServer.arg(F("uhp")).toInt();   // Upload Host Port
    if (webServer.hasArg(F("url"))) conf.url = webServer.arg(F("url"));           // Upload Host URL
    if (webServer.hasArg(F("lgf"))) conf.lgf = webServer.arg(F("lgf"))=="1";      // Bool: logging to file
    if (webServer.hasArg(F("lgs"))) conf.lgs = webServer.arg(F("lgs"))=="1";      // Bool: logging to Serial
    if (webServer.hasArg(F("gdl"))) conf.gdl = webServer.arg(F("gdl"))=="1";      // Bool: GPS logging
    if (webServer.hasArg(F("wso"))) conf.wso = webServer.arg(F("wso"))=="1";      // Bool: autom. WiFi-Start
    if (webServer.hasArg(F("gcf"))) conf.gcf = webServer.arg(F("gcf"));           // GPS config file
    if (webServer.hasArg(F("fbe"))) conf.fbe = webServer.arg(F("fbe"));           // File Browser & Editor
    conf.write();
    checkFilesystemSpace(); // currentfpw wird dort ggf. korrigiert
    handleMessage(F("Konfiguration gesichert."), false);
  }
}

/***************************************************
 * webHandler:
 * Ausgabe von runtime Infos
 ***************************************************/
void handleDebugInfo(void) {
  webServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  webServer.send ( 200, TEXT_HTML, F("<pre>Debug Info:\n"));

  // runtime info:
  long laufzeit = millis();
  int temp = (laufzeit/3600000);  // Stunden
  String sZeit = temp < 10 ? "0" + String(temp) : String(temp);
  temp = (laufzeit/60000) % 60;   // Minuten
  sZeit += ":" + (temp < 10 ? "0" + String(temp) : String(temp));
  temp = (laufzeit/1000) % 60;
  sZeit += ":" + (temp < 10 ? "0" + String(temp) : String(temp));

  webServer.sendContent(F("\n  runtime info:\n"));
  webServer.sendContent("    Laufzeit........:" + sZeit + "\n");
  webServer.sendContent("    Free RAM........:" + String(ESP.getFreeHeap()) + "\n");
  webServer.sendContent("    charsProcessed..:" + String(gpsNew.charsProcessed()) + "\n");
  webServer.sendContent("    sentencesWithFix:" + String(gpsNew.sentencesWithFix()) + "\n");
  webServer.sendContent("    failedChecksum..:" + String(gpsNew.failedChecksum()) + "\n");
  webServer.sendContent("    passedChecksum..:" + String(gpsNew.passedChecksum()) + "\n");
  webServer.sendContent("    meterSincePump..:" + String(meterSincePump) + "\n");
  #ifdef _NO_PUMP_
    webServer.sendContent(F("    pumpMode........:DEAKTIVIERT!\n"));
  #else
    webServer.sendContent("    pumpMode........:" + getPumpModeStr(pumpMode) + "\n");
  #endif
  webServer.sendContent("    - Filename......:" + String(gpsTrackFilename) + "\n");
  webServer.sendContent("    - GPS records...:" + String(totalGPSwrite) + "\n");
  webServer.sendContent("    - write errors..:" + String(totalWriteErrors) + "\n");
  webServer.sendContent("    Total GPS Meters:" + String(totalDist) + "\n");
  webServer.sendContent("    movementCounter.:" + String(movementCounter) + " (max " + String(MAX_MOVEMENT_COUNT) + ")\n");
  webServer.sendContent("    Total pump count:" + String(totalPumpCount) + "\n");
  webServer.sendContent("    maintenance mode:" + String(maintenanceMode ? "Ja\n" : "Nein\n"));
  webServer.sendContent("    Wifi connected..:" + WiFi.SSID() + "\n");
  webServer.sendContent("    IP Adress.......:" + WiFi.localIP().toString() + "\n");

  // Einstellungen:
  webServer.sendContent(F("  Konfiguration:\n"));
  webServer.sendContent("    Version............. = " + String(VERSION) + "\n");
#ifdef _DISPLAY_AVAILABLE_  
  webServer.sendContent(F("    Display............. = Ja\n"));
#else  
  webServer.sendContent(F("    Display............. = Nein\n"));
#endif  
  webServer.sendContent("    Simulation wait time = " + String(conf.wts) + "\n");
  webServer.sendContent("    Normal Mode Meter... = " + String(conf.nmm) + "\n");
  webServer.sendContent("    Regen Mode Meter.... = " + String(conf.rmm) + "\n");
  webServer.sendContent("    Offroad Mode Meter.. = " + String(conf.omm) + "\n");
  webServer.sendContent("    Simulation Meter.... = " + String(conf.sim) + "\n");
  webServer.sendContent("    GPS Low Filter (cm). = " + String(conf.glf) + "\n");
  webServer.sendContent("    Pump action count... = " + String(conf.pac) + "\n");
  webServer.sendContent("    Tank Kapazit&auml;t...... = " + String(conf.otk) + "\n");
  webServer.sendContent("    ...davon verbraucht. = " + String(conf.use) + "\n");
  webServer.sendContent("    firmware filename... = \"" + String(conf.ffn) + "\"\n");
  webServer.sendContent("    (default f.this MCU) = \"" + String(UPDATE_FILE) + "\"\n");
  webServer.sendContent("    Abstand GPS writes s = " + String(conf.fpw) + "\n");
  webServer.sendContent("    AP- und Hostname.... = \"" + String(conf.apn) + "\"\n");
  webServer.sendContent("    AP Passwort......... = \"" + String(conf.app) + "\"\n");
  webServer.sendContent("    Upload Host Name.... = \"" + String(conf.uhn) + "\"\n");
  webServer.sendContent("    Upload Host Port.... = " + String(conf.uhp) + "\n");
  webServer.sendContent("    Upload Host URL..... = \"" + String(conf.url) + "\"\n");
  webServer.sendContent("    Logging to file..... = " + String(conf.lgf == 1 ? "ja" : "nein") + "\n");
  webServer.sendContent("    Logging to Serial... = " + String(conf.lgs == 1 ? "ja" : "nein") + "\n");
  webServer.sendContent("    GPS Logging to file. = " + String(conf.gdl == 1 ? "ja" : "nein") + "\n");
  webServer.sendContent("    WiFi bei Start an... = " + String(conf.wso == 1 ? "ja" : "nein") + "\n");
  webServer.sendContent("    GPS config file..... = \"" + String(conf.gcf) + "\"\n");
  webServer.sendContent("    FS browser & editor. = \"" + String(conf.fbe) + "\"\n");

  // Inhalt LittleFS:
  webServer.sendContent(F("\n  LittleFS Dateien:\n<table>"));
  Dir dir = _FILESYS.openDir("/");
  while (dir.next()) {
    webServer.sendContent("<tr><td>" + dir.fileName() + "</td><td align=right>" + String(dir.fileSize()) + "</td></tr>");
  }
  webServer.sendContent(F("</table>\n"));

  FSInfo fs_info;
  if (_FILESYS.info(fs_info)) {
    uint32_t freespace = fs_info.totalBytes - (uint32_t)(fs_info.usedBytes * 1.05);
    webServer.sendContent("  Free space (in bytes) : " + String(freespace) + "\n");
    uint32_t gpsrec = freespace / GPS_RECORD_SIZE;    // Platz für (gpsrec) GPS Records
    webServer.sendContent("  genug fuer GPS records: " + String(gpsrec) + "\n");
    if (conf.fpw > 0) {
      uint32_t gpsmin = (gpsrec * conf.fpw) / 60;     // Umrechnung in Minuten
      webServer.sendContent("  reicht f&uuml;r " + String(gpsmin / 60) + " Stunden und "+String(gpsmin % 60)+" Minuten\n");
    }
    webServer.sendContent("  track recording aktuell aktiv: " + String(currentfpw == 0 ? "Nein" : "Ja") + "\n</pre>");
  }
  
  webServer.sendContent("");  // this tells web client that transfer is done
  webServer.client().stop();  // ??
}
