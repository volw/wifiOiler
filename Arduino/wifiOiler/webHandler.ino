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

/*****************************************************************
 * setup Web Server handler
 *****************************************************************/
void setupWebServer(void) {
  //list directory
  GVwebServer.on(F("/list"), HTTP_GET, handleFileList);
  GVwebServer.on(F("/info"), HTTP_GET, handleDebugInfo);
  GVwebServer.on(F("/pumptest"), HTTP_GET, handlePumpTest);
  GVwebServer.on(F("/ledtest"), HTTP_GET, handleLEDTest);
  GVwebServer.on(F("/config"), HTTP_GET, handleConfigPage);
  GVwebServer.on(F("/config"), HTTP_POST, handleConfigPage);
  GVwebServer.on(F("/upload"), HTTP_GET, handleUpload);
  GVwebServer.on(F("/pumpmode"), HTTP_GET, handlePumpMode);
  GVwebServer.on(F("/reboot"), HTTP_GET, handleReboot);
  GVwebServer.on(F("/update"), HTTP_GET, handleUpdate);
  GVwebServer.on(F("/version"), HTTP_GET, handleVersion);
 
  //LittleFS Browser & editor:
  GVwebServer.on(F("/edit"), HTTP_GET, []() {
    if (!handleFileRead(GVoilerConf.fbe)) {
      GVwebServer.send(404, TEXT_PLAIN, F("FileNotFound"));
    }
  });
  GVwebServer.on(F("/edit"), HTTP_PUT, handleFileCreate);     //create file
  GVwebServer.on(F("/edit"), HTTP_DELETE, handleFileDelete);  //delete file
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  GVwebServer.on(F("/edit"), HTTP_POST, []() {
    GVwebServer.send(200, TEXT_PLAIN, "");
  }, handleFileUpload);

  GVwebServer.onNotFound([]() {
    if (GVwebServer.uri().equalsIgnoreCase("/connecttest.txt")){
      DEBUG_OUT.print(C_SERVE_INLINE);
      GVwebServer.send(200, TEXT_PLAIN, C_CONNECTTEST);
    } else if (GVwebServer.uri().equalsIgnoreCase("/hotspot-detect.html")){
      DEBUG_OUT.print(C_SERVE_INLINE);
      GVwebServer.send(200, TEXT_HTML, C_HOTSPOT_DETECT);
    } else {
      if (!handleFileRead(GVwebServer.uri())) {
        DEBUG_OUT.print(C_FILENOTFOUND);
        GVwebServer.send(404, TEXT_PLAIN, F("FileNotFound"));
      } else DEBUG_OUT.print(C_SERVEFILE);
    }
    DEBUG_OUT.println(GVwebServer.uri());
  });
  GVwebServer.begin();
}


/*************************************************
 * Content type anhand Dateiendung identifizieren
 *************************************************/
String getContentType(String fname) {
  if (GVwebServer.hasArg(F("download"))) return F("application/octet-stream");
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
  //DEBUG_OUT.println("[handleFileRead] " + path);
  // wenn keine Datei mitgegeben wurde, nehmen wir index.htm an:
  if (path.endsWith("/")) path += "index.htm";
  if (!path.startsWith("/")) path = "/"+path;

  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  
  if (_FILESYS.exists(pathWithGz) || _FILESYS.exists(path)) {
    File file;  
    if (_FILESYS.exists(path))
      file = _FILESYS.open(path, "r");
    else 
      file = _FILESYS.open(pathWithGz, "r");

    if (file) {
      GVwebServer.streamFile(file, contentType);
      file.close();
    }
    else 
    {
      DEBUG_OUT.print(F(MSG_DBG_WEB_FILE_READ_ERROR));
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
void handleMessage(String message, bool justBack)
{
  GVwebServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  GVwebServer.send( 200, TEXT_HTML, "");
  //GVwebServer.sendContent(F("<!DOCTYPE html><html><head>"));
  GVwebServer.sendContent(F("<meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\">"));
  //GVwebServer.sendContent(F("</head><body><script type=\"text/javascript\">"));
  GVwebServer.sendContent(F("</head><body><script>function msg(){alert(\""));
  GVwebServer.sendContent(message);
  if (justBack)
    GVwebServer.sendContent(F("\");history.back();}"));
  else
    GVwebServer.sendContent(F("\");window.location.replace('/');}"));
  GVwebServer.sendContent(F("window.onload=msg;</script></body></html>"));
  GVwebServer.sendContent(""); // this tells web client that transfer is done
  GVwebServer.client().stop();
}

/***************************************************
 * webHandler: Test Pumpe
 ***************************************************/
void handlePumpTest(void)
{
  TriggerPump();
  handleMessage(F("Pumpentest gestartet"));
}

/***************************************************
 * webHandler: Test LED
 ***************************************************/
void handleLEDTest(void)
{
  GVmyLedx.add LED_TEST_GRUEN;
  GVmyLedx.add (0, 300);
  GVmyLedx.add LED_TEST_ROT;
  GVmyLedx.start();
  handleMessage(F("LED Test gestartet (erst gruen, dann rot)"));
}

/***************************************************
 * Gibt die aktuelle Version als plain text zurück
 ***************************************************/
void handleVersion(void) {
  GVwebServer.send(200, TEXT_PLAIN, GVoilerConf.apn+" v"+String(VERSION));
}

/***************************************************
 * webHandler: Reboot oiler
 * serve html which initiates reboot...
 ***************************************************/
void handleReboot(void)
{
  if (GVwebServer.hasArg(F("bootnow"))){
    GVmyLedx.on(LED_ROT);
    GVmyDisplay.PrintMessage("Reboot...");
    //handleMessage(F("rebooting..."));
    GVwebServer.send( 200, TEXT_HTML, "OK" );
    GVwebServer.handleClient();
    
    //uint32_t wait = millis();
    //while (wait + 1000 > millis()) GVwebServer.handleClient();
    ESP.restart();
  } else handleFileRead("/reboot.htm");
}

/***************************************************
 * Interaktives Umschalten der Pump Modi
 ***************************************************/
void handlePumpMode(void) {
  if (GVwebServer.hasArg(F("mode")))
  {
    String modestr = GVwebServer.arg(F("mode"));
    if (modestr.equalsIgnoreCase(getPumpModeStr(MODE_NORMAL))) setNewMode(MODE_NORMAL);
    else if (modestr.equalsIgnoreCase(getPumpModeStr(MODE_REGEN))) setNewMode(MODE_REGEN);
    else if (modestr.equalsIgnoreCase(getPumpModeStr(MODE_OFFROAD))) setNewMode(MODE_OFFROAD);
    else if (modestr.equalsIgnoreCase(getPumpModeStr(MODE_PERMANENT))) setNewMode(MODE_PERMANENT);
    else if (modestr.equalsIgnoreCase(getPumpModeStr(MODE_OFF))) setNewMode(MODE_OFF);
    else
    {
      return GVwebServer.send(200, TEXT_PLAIN, "UNKNOWN");
    }
    return GVwebServer.send(200, TEXT_PLAIN, "OK");
  }
  else // return current pumpmode
  {
    GVwebServer.send(200, TEXT_PLAIN, getPumpModeStr(GVpumpMode));
  }
}

/***********************************************************
 * Liefern aller Config Daten für die entsprechende Web-Page
 * oder
 * parsen der zurückgegebenen Config Werte und speichern
 ***********************************************************/
void handleConfigPage(void) {
  if (GVwebServer.args() == 0)
  {
    // sending config values as json string:
    String output = "{\"wts\":\"" + String(GVoilerConf.wts) + "\"";
    output += ",\"nmm\":\"" + String(GVoilerConf.nmm) + "\"";
    output += ",\"rmm\":\"" + String(GVoilerConf.rmm) + "\"";
    output += ",\"omm\":\"" + String(GVoilerConf.omm) + "\"";
    output += ",\"sim\":\"" + String(GVoilerConf.sim) + "\"";
    output += ",\"pac\":\"" + String(GVoilerConf.pac) + "\"";
    output += ",\"glf\":\"" + String(GVoilerConf.glf) + "\"";
    output += ",\"otk\":\"" + String(GVoilerConf.otk) + "\"";
    output += ",\"use\":\"" + String(GVoilerConf.use) + "\"";
    output += ",\"ffn\":\"" + String(GVoilerConf.ffn) + "\"";
    output += ",\"fpw\":\"" + String(GVoilerConf.fpw) + "\"";
    output += ",\"apn\":\"" + String(GVoilerConf.apn) + "\"";
    output += ",\"app\":\"" + String(GVoilerConf.app) + "\"";
    output += ",\"uhn\":\"" + String(GVoilerConf.uhn) + "\"";
    output += ",\"uhp\":\"" + String(GVoilerConf.uhp) + "\"";
    output += ",\"url\":\"" + String(GVoilerConf.url) + "\"";
    output += ",\"lgf\":\"" + String(GVoilerConf.lgf) + "\"";
    output += ",\"lgs\":\"" + String(GVoilerConf.lgs) + "\"";
    output += ",\"gdl\":\"" + String(GVoilerConf.gdl) + "\"";
    output += ",\"wso\":\"" + String(GVoilerConf.wso) + "\"";
    output += ",\"gcf\":\"" + String(GVoilerConf.gcf) + "\"";
    output += ",\"fbe\":\"" + String(GVoilerConf.fbe) + "\"}";
    GVwebServer.send(200, F("text/json"), output);
  } else {
    // Konfiguration aus den Argumenten lesen
    if (GVwebServer.hasArg(F("wts"))) GVoilerConf.wts = GVwebServer.arg(F("wts")).toInt();   // Wartezeit Simulation (s)
    if (GVwebServer.hasArg(F("nmm"))) GVoilerConf.nmm = GVwebServer.arg(F("nmm")).toInt();   // Normal Mode Meters
    if (GVwebServer.hasArg(F("rmm"))) GVoilerConf.rmm = GVwebServer.arg(F("rmm")).toInt();   // Regen Mode Meters
    if (GVwebServer.hasArg(F("omm"))) GVoilerConf.omm = GVwebServer.arg(F("omm")).toInt();   // Offroad Mode Meters
    if (GVwebServer.hasArg(F("sim"))) GVoilerConf.sim = GVwebServer.arg(F("sim")).toInt();   // Simulation Meter (Notprogramm)
    if (GVwebServer.hasArg(F("pac"))) GVoilerConf.pac = GVwebServer.arg(F("pac")).toInt();   // Pump Action Count
    if (GVwebServer.hasArg(F("glf"))) GVoilerConf.glf = GVwebServer.arg(F("glf")).toInt();   // GPS Low-Filter
    if (GVwebServer.hasArg(F("otk"))) GVoilerConf.otk = GVwebServer.arg(F("otk")).toInt();   // Tank Kapazität
    if (GVwebServer.hasArg(F("use"))) GVoilerConf.use = GVwebServer.arg(F("use")).toInt();   // ..davon verbraucht
    if (GVwebServer.hasArg(F("ffn"))) GVoilerConf.ffn = GVwebServer.arg(F("ffn"));           // Firmware FileName
    if (GVwebServer.hasArg(F("fpw"))) GVoilerConf.fpw = GVwebServer.arg(F("fpw")).toInt();   // Abstand zwischen GPS writes (s)
    if (GVwebServer.hasArg(F("apn"))) GVoilerConf.apn = GVwebServer.arg(F("apn"));           // Access Point Name
    if (GVwebServer.hasArg(F("app"))) GVoilerConf.app = GVwebServer.arg(F("app"));           // Access Point Passwort
    if (GVwebServer.hasArg(F("uhn"))) GVoilerConf.uhn = GVwebServer.arg(F("uhn"));           // Upload Host Name
    if (GVwebServer.hasArg(F("uhp"))) GVoilerConf.uhp = GVwebServer.arg(F("uhp")).toInt();   // Upload Host Port
    if (GVwebServer.hasArg(F("url"))) GVoilerConf.url = GVwebServer.arg(F("url"));           // Upload Host URL
    if (GVwebServer.hasArg(F("lgf"))) GVoilerConf.lgf = GVwebServer.arg(F("lgf"))=="1";      // Bool: logging to file
    if (GVwebServer.hasArg(F("lgs"))) GVoilerConf.lgs = GVwebServer.arg(F("lgs"))=="1";      // Bool: logging to Serial
    if (GVwebServer.hasArg(F("gdl"))) GVoilerConf.gdl = GVwebServer.arg(F("gdl"))=="1";      // Bool: GPS logging
    if (GVwebServer.hasArg(F("wso"))) GVoilerConf.wso = GVwebServer.arg(F("wso"))=="1";      // Bool: autom. WiFi-Start
    if (GVwebServer.hasArg(F("gcf"))) GVoilerConf.gcf = GVwebServer.arg(F("gcf"));           // GPS config file
    if (GVwebServer.hasArg(F("fbe"))) GVoilerConf.fbe = GVwebServer.arg(F("fbe"));           // File Browser & Editor
    GVoilerConf.write();
    checkFilesystemSpace(); // GVcurrentfpw wird dort ggf. korrigiert
    handleMessage(F("Konfiguration gesichert."), false);
  }
}

/***************************************************
 * webHandler:
 * Ausgabe von runtime Infos
 ***************************************************/
void handleDebugInfo(void) {
  GVwebServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  GVwebServer.send ( 200, TEXT_HTML, F("<pre>Debug Info:\n"));

  // runtime info:
  long laufzeit = millis();
  int temp = (laufzeit/3600000);  // Stunden
  String sZeit = temp < 10 ? "0" + String(temp) : String(temp);
  temp = (laufzeit/60000) % 60;   // Minuten
  sZeit += ":" + (temp < 10 ? "0" + String(temp) : String(temp));
  temp = (laufzeit/1000) % 60;
  sZeit += ":" + (temp < 10 ? "0" + String(temp) : String(temp));

  GVwebServer.sendContent(F("\n  runtime info:\n"));
  GVwebServer.sendContent("    Laufzeit........:" + sZeit + "\n");
  GVwebServer.sendContent("    Free RAM........:" + String(ESP.getFreeHeap()) + "\n");
  GVwebServer.sendContent("    charsProcessed..:" + String(GVgpsNew.charsProcessed()) + "\n");
  GVwebServer.sendContent("    sentencesWithFix:" + String(GVgpsNew.sentencesWithFix()) + "\n");
  GVwebServer.sendContent("    failedChecksum..:" + String(GVgpsNew.failedChecksum()) + "\n");
  GVwebServer.sendContent("    passedChecksum..:" + String(GVgpsNew.passedChecksum()) + "\n");
  GVwebServer.sendContent("    GVmeterSincePump:" + String(GVmeterSincePump) + "\n");
  #ifdef _NO_PUMP_
    GVwebServer.sendContent(F("    pumpMode........:DEAKTIVIERT!\n"));
  #else
    GVwebServer.sendContent("    pumpMode........:" + getPumpModeStr(GVpumpMode) + "\n");
  #endif
  GVwebServer.sendContent("    - Filename......:" + String(GVgpsTrackFilename) + "\n");
  GVwebServer.sendContent("    - GPS records...:" + String(GVtotalGPSwrite) + "\n");
  GVwebServer.sendContent("    - write errors..:" + String(GVtotalWriteErrors) + "\n");
  GVwebServer.sendContent("    Total GPS Meters:" + String(GVtotalDist) + "\n");
  GVwebServer.sendContent("   GVmovementCounter:" + String(GVmovementCounter) + " (max " + String(MAX_MOVEMENT_COUNT) + ")\n");
  GVwebServer.sendContent("    Total pump count:" + String(GVtotalPumpCount) + "\n");
  GVwebServer.sendContent("    maintenance mode:" + String(GVmaintenanceMode ? "Ja\n" : "Nein\n"));
  GVwebServer.sendContent("    Wifi connected..:" + WiFi.SSID() + "\n");
  GVwebServer.sendContent("    IP Adress.......:" + WiFi.localIP().toString() + "\n");

  // Einstellungen:
  GVwebServer.sendContent(F("  Konfiguration:\n"));
  GVwebServer.sendContent("    Version............. = " + String(VERSION) + "\n");
#ifdef _DISPLAY_AVAILABLE_  
  GVwebServer.sendContent(F("    Display............. = Ja\n"));
#else  
  GVwebServer.sendContent(F("    Display............. = Nein\n"));
#endif  
  GVwebServer.sendContent("    Simulation wait time = " + String(GVoilerConf.wts) + "\n");
  GVwebServer.sendContent("    Normal Mode Meter... = " + String(GVoilerConf.nmm) + "\n");
  GVwebServer.sendContent("    Regen Mode Meter.... = " + String(GVoilerConf.rmm) + "\n");
  GVwebServer.sendContent("    Offroad Mode Meter.. = " + String(GVoilerConf.omm) + "\n");
  GVwebServer.sendContent("    Simulation Meter.... = " + String(GVoilerConf.sim) + "\n");
  GVwebServer.sendContent("    GPS Low Filter (cm). = " + String(GVoilerConf.glf) + "\n");
  GVwebServer.sendContent("    Pump action count... = " + String(GVoilerConf.pac) + "\n");
  GVwebServer.sendContent("    Tank Kapazit&auml;t...... = " + String(GVoilerConf.otk) + "\n");
  GVwebServer.sendContent("    ...davon verbraucht. = " + String(GVoilerConf.use) + "\n");
  GVwebServer.sendContent("    firmware filename... = \"" + String(GVoilerConf.ffn) + "\"\n");
  GVwebServer.sendContent("    (default f.this MCU) = \"" + String(UPDATE_FILE) + "\"\n");
  GVwebServer.sendContent("    Abstand GPS writes s = " + String(GVoilerConf.fpw) + "\n");
  GVwebServer.sendContent("    AP- und Hostname.... = \"" + String(GVoilerConf.apn) + "\"\n");
  GVwebServer.sendContent("    AP Passwort......... = \"" + String(GVoilerConf.app) + "\"\n");
  GVwebServer.sendContent("    Upload Host Name.... = \"" + String(GVoilerConf.uhn) + "\"\n");
  GVwebServer.sendContent("    Upload Host Port.... = " + String(GVoilerConf.uhp) + "\n");
  GVwebServer.sendContent("    Upload Host URL..... = \"" + String(GVoilerConf.url) + "\"\n");
  GVwebServer.sendContent("    Logging to file..... = " + String(GVoilerConf.lgf == 1 ? "ja" : "nein") + "\n");
  GVwebServer.sendContent("    Logging to Serial... = " + String(GVoilerConf.lgs == 1 ? "ja" : "nein") + "\n");
  GVwebServer.sendContent("    GPS Logging to file. = " + String(GVoilerConf.gdl == 1 ? "ja" : "nein") + "\n");
  GVwebServer.sendContent("    WiFi bei Start an... = " + String(GVoilerConf.wso == 1 ? "ja" : "nein") + "\n");
  GVwebServer.sendContent("    GPS config file..... = \"" + String(GVoilerConf.gcf) + "\"\n");
  GVwebServer.sendContent("    FS browser & editor. = \"" + String(GVoilerConf.fbe) + "\"\n");

  // Inhalt LittleFS:
  GVwebServer.sendContent(F("\n  LittleFS Dateien:\n<table>"));
  Dir dir = _FILESYS.openDir("/");
  while (dir.next()) {
    GVwebServer.sendContent("<tr><td>" + dir.fileName() + "</td><td align=right>" + String(dir.fileSize()) + "</td></tr>");
  }
  GVwebServer.sendContent(F("</table>\n"));

  FSInfo fs_info;
  if (_FILESYS.info(fs_info)) {
    uint32_t freespace = fs_info.totalBytes - (uint32_t)(fs_info.usedBytes * 1.05);
    GVwebServer.sendContent("  Free space (in bytes) : " + String(freespace) + "\n");
    uint32_t gpsrec = freespace / GPS_RECORD_SIZE;    // Platz für (gpsrec) GPS Records
    GVwebServer.sendContent("  genug fuer GPS records: " + String(gpsrec) + "\n");
    if (GVoilerConf.fpw > 0) {
      uint32_t gpsmin = (gpsrec * GVoilerConf.fpw) / 60;     // Umrechnung in Minuten
      GVwebServer.sendContent("  reicht f&uuml;r " + String(gpsmin / 60) + " Stunden und "+String(gpsmin % 60)+" Minuten\n");
    }
    GVwebServer.sendContent("  track recording aktuell aktiv: " + String(GVcurrentfpw == 0 ? "Nein" : "Ja") + "\n</pre>");
  }
  
  GVwebServer.sendContent("");  // this tells web client that transfer is done
  GVwebServer.client().stop();  // ??
}
