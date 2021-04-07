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
  GVwebServer.on(F("/info"), HTTP_GET, handleRunTimeInfo);
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
 * wait some ms and five webserver time to respond
 * only necessary if not returning to main loop
 ***************************************************/
void handleClient(uint32_t waitms=500)
{
  uint32_t wait = millis();
  while (wait + waitms > millis()) GVwebServer.handleClient();
}

/***************************************************
 * webHandler: Test Pumpe
 ***************************************************/
void handlePumpTest(void)
{
  GVwebServer.send( 200, TEXT_HTML, "OK" );
  handleClient();
  TriggerPump();
}

/***************************************************
 * webHandler: Test LED
 ***************************************************/
void handleLEDTest(void)
{
  GVwebServer.send( 200, TEXT_HTML, "OK" );
  handleClient();

  GVmyLedx.add LED_TEST_GRUEN;
  GVmyLedx.add (0, 300);
  GVmyLedx.add LED_TEST_ROT;
  GVmyLedx.start();
  //handleMessage(F("LED Test gestartet (erst gruen, dann rot)"));
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
    //GVwebServer.send( 200, TEXT_HTML, "OK" );
    //handleClient();

    ESP.restart();
  } GVwebServer.send( 400, TEXT_HTML, BAD_ARGS );
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
      return GVwebServer.send(400, TEXT_PLAIN, "UNKNOWN");
    }
    GVwebServer.send(200, TEXT_PLAIN, getPumpModeStr(GVpumpMode));
  }
  else // return current pumpmode
  {
    GVwebServer.send(200, TEXT_PLAIN, getPumpModeStr(GVpumpMode));
  }
}

/***********************************************************
 * Ggf. Speichern der übergebenen Werte
 * Rückgabe der aktuellen Konfiguration as json
 ***********************************************************/
void handleConfigPage(void) {
  if (GVwebServer.args() > 0)
  {
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
    if (GVwebServer.hasArg(F("fbe"))) GVoilerConf.fbe = GVwebServer.arg(F("fbe"));           // File Browser & Editor
    GVoilerConf.write();
    checkFilesystemSpace(); // GVcurrentfpw wird dort ggf. korrigiert
  }

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
  output += ",\"fbe\":\"" + String(GVoilerConf.fbe) + "\"}";
  GVwebServer.send(200, F("text/json"), output);
}

/***************************************************
 * webHandler:
 * Ausgabe von runtime Infos
 ***************************************************/
void handleRunTimeInfo(void) {
  // runtime info:
  long laufzeit = millis();
  int temp = (laufzeit/3600000);  // Stunden
  String sZeit = temp < 10 ? "0" + String(temp) : String(temp);
  temp = (laufzeit/60000) % 60;   // Minuten
  sZeit += ":" + (temp < 10 ? "0" + String(temp) : String(temp));
  temp = (laufzeit/1000) % 60;
  sZeit += ":" + (temp < 10 ? "0" + String(temp) : String(temp));

  String output = "{\"ve\":\"" + String(VERSION) + "\"";  // version
  output += ",\"rt\":\"" + sZeit + "\"";          // runtime
  #ifdef _DISPLAY_AVAILABLE_  
    output += ",\"da\":\"Ja\"";                   // display available
  #else
    output += ",\"da\":\"Nein\"";
  #endif    

  FSInfo fs_info;
  uint32_t freespace = 0, gpsrec = 0, gpsmin = 0;
  if (_FILESYS.info(fs_info)) {
    freespace = fs_info.totalBytes - (uint32_t)(fs_info.usedBytes * 1.05);
    gpsrec = freespace / GPS_RECORD_SIZE;    // Platz für (gpsrec) GPS Records
    if (GVoilerConf.fpw > 0) {
      gpsmin = (gpsrec * GVoilerConf.fpw) / 60;     // Umrechnung in Minuten
      sZeit = String(gpsmin / 60) + ":";
      temp = gpsmin % 60;
      sZeit += temp < 10 ? "0"+ String(temp) : String(temp);
    }
  }

  // sending config values as json string:
  output += ",\"fr\":" + String(ESP.getFreeHeap());                   // free ram
  output += ",\"cr\":" + String(GVgpsNew.charsProcessed());           // gps characters processed
  output += ",\"gf\":" + String(GVgpsNew.sentencesWithFix());         // gps sentences with fix
  output += ",\"pc\":" + String(GVgpsNew.passedChecksum());           // gps sentences with correct checksum
  output += ",\"fc\":" + String(GVgpsNew.failedChecksum());           // gps sentences with failed checksum
  output += ",\"mp\":" + String(GVmeterSincePump);                    // total gps meters since start
  #ifdef _NO_PUMP_
    output += ",\"pm\":\"DEAKTIVIERT!\"";
  #else
    output += ",\"pm\":\"" + getPumpModeStr(GVpumpMode) + "\"";       // current pump mode
  #endif
  output += ",\"ra\":\"" + String(GVcurrentfpw == 0 ? "Nein" : "Ja") + "\""; // track recording active
  output += ",\"tf\":\"" + String(GVgpsTrackFilename) + "\"";         // track file name
  output += ",\"rw\":" + String(GVtotalGPSwrite);                     // no of gps records written
  output += ",\"rs\":" + String(GPS_RECORD_SIZE);                     // size of gps record
  output += ",\"we\":" + String(GVtotalWriteErrors);                  // count of gps record write errors
  output += ",\"gm\":" + String(GVtotalDist);                         // total gps meters since start
  output += ",\"mc\":" + String(GVmovementCounter);                   // current state of movement counter
  output += ",\"xc\":" + String(MAX_MOVEMENT_COUNT);                  // max movement counter
  output += ",\"tc\":" + String(GVtotalPumpCount);                    // no of pump actions since start
  output += ",\"mm\":\"" + String(GVmaintenanceMode ? "Ja" : "Nein") + "\"";  // maintenance mode active
  output += ",\"wc\":\"" + WiFi.SSID() + "\"";                        // connected wifi (ssid)
  output += ",\"ip\":\"" + WiFi.localIP().toString() + "\"";          // current ip adress
  output += ",\"fs\":" + String(freespace);                           // free space in file system (bytes)
  output += ",\"rl\":" + String(gpsrec);                              // left space/capacity for x gps records
  output += ",\"tl\":\"" + sZeit + "\"}";                             // left time for track recording
  GVwebServer.send(200, F("text/json"), output);
}