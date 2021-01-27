//**** Update Oiler (upload files and restart on new binary ***************
#define UPD_NOT_ACTIVE       0
#define UPD_UPLOADING_FILES  1
#define UPD_UPDATE_ENDED     2
#define UPD_MUST_REBOOT      3
#define UPD_REBOOT_NOW       4
#define UPD_ERROR            5
String   updateMessage    = "";     // response string for web gui
uint8_t  updateResult     = UPD_NOT_ACTIVE;   // state of update (s. defines)
uint32_t updFileSizeTotal = 0;     // upload progress, total size (bytes) of current file
uint32_t updSizeUploaded  = 0;     // upload progress, uploaded number of bytes of current file
bool     updError         = false; // some error during uploading process (file possibly not complete)
#define _UF_INITIAL  0
#define _UF_UPLOADOK 1
#define _UF_UPLOAD_ERROR 2
struct updateFile {
  String fileName;
  uint8_t state;
};
std::vector<updateFile> updateFiles;

HTTPClient http;
WiFiClient client = webServer.client();

/**************************************************************
 * check for new binary in file system (file name see <conf.ffn>)
 * when found: update & restart
 **************************************************************/
bool checkforUpdate(bool justCheck, bool reboot) {
  bool update = false;

  String FirmwareFilename = conf.ffn;
  if (!FirmwareFilename.startsWith("/")) FirmwareFilename = "/" + conf.ffn;
  DEBUG_OUT.print(F("[checkforUpdate] Looking for firmware file "));
  DEBUG_OUT.println(conf.ffn);
  if (justCheck) return(_FILESYS.exists(FirmwareFilename));
  
  // ist justCheck == false, wird Firmware eingespielt
  if (_FILESYS.exists(FirmwareFilename))
  {
    DEBUG_OUT.println(F("Firmware file found - updating now..."));
    myDisplay.PrintMessage("Firmware\nfound:\nupdating..");
    File file = _FILESYS.open(FirmwareFilename, "r");
  
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    if (!Update.begin(maxSketchSpace, U_FLASH)) { //start with max available size
      Update.printError(DEBUG_OUT);
      DEBUG_OUT.println(F("[checkforUpdate] ERROR"));
    }
    else
    {
      while (file.available())
      {
        myLedx.on(LED_GRUEN);
        uint8_t ibuffer[128];
        file.read((uint8_t *)ibuffer, 128);
        Update.write(ibuffer, sizeof(ibuffer));
        myLedx.off();
      }
      DEBUG_OUT.print(F("[checkforUpdate] Update.end(true)=="));
      update = Update.end(true);
      DEBUG_OUT.println(update);
      if (!update) {
        DEBUG_OUT.print(F("[checkforUpdate] Update.getError():"));
        DEBUG_OUT.println(Update.getError());
      }
    }
    file.close();
    DEBUG_OUT.print(F("[checkforUpdate] removing firmware file "));
    DEBUG_OUT.println(conf.ffn);
    if (!_FILESYS.remove(FirmwareFilename)) DEBUG_OUT.println(F("ERROR removing firmware file!!"));
    if (update && reboot)
    {
      myDisplay.PrintMessage(F("Reboot..."));
      DEBUG_OUT.println(F("[checkforUpdate] rebooting after update..."));
      delay(10);
      ESP.restart();
    }
    else myDisplay.MessageOff();
  }
  else 
  {
    DEBUG_OUT.println("[checkforUpdate] no new firmware found");
  }
}

/***********************************************************
 * Get update info from oilerbase (when available)
 * BOARD_TYPE, VERSION in URL for php script on oilerbase
 * expecting result: list of update files (separated by ';')
 ***********************************************************/ 
bool getUpdateInfo(void)
{
  updateFiles.clear();  // empty vector
  String url = "http://" + conf.uhn + "/update.php?board="+BOARD_TYPE+"&version="+VERSION;
  String result = "";

  http.setUserAgent(F("wifiOiler"));
  http.begin(client, url);

  int httpCode = http.GET();
  DEBUG_OUT.println("[getUpdateInfo] URL = " + url);
  DEBUG_OUT.print(F("[getUpdateInfo] http.GET() response: "));
  DEBUG_OUT.println(httpCode);
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    result = http.getString();  // erstes und letztes Zeichen '\n', warum????? (was the php on oilerbase, don't know why)
    DEBUG_OUT.print(F("[getUpdateInfo] http.GET() returned: "));
    DEBUG_OUT.println(result);
////>>>>debug
//    for (int i = 0; i < result.length(); i++){
//      DEBUG_OUT.print("Zeichen(code) an Stelle ");
//      DEBUG_OUT.print(i, DEC);
//      DEBUG_OUT.print("=");
//      DEBUG_OUT.print(result.charAt(i), DEC);
//      DEBUG_OUT.print(", ");
//      DEBUG_OUT.println(result.charAt(i), HEX);
//    }
////<<<<debug
    if (httpCode == 200){ // if valid update info received, fill (vector) updateFiles
      int16_t nPos; // DO NOT USE uint.. TYPE HERE (as I did)
      while ((nPos=result.indexOf(';')) > 0) {
        updateFiles.push_back({result.substring(0, nPos),_UF_INITIAL});
        result = result.substring(nPos+1);
      }
      updateFiles.push_back({result,_UF_INITIAL});
    }
  }
  else
  {
    DEBUG_OUT.printf("[getUpdateInfo] http.GET() failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end(); 
  return (updateFiles.size() > 0);
}

/**************************************************
 * Request file (subPath) from oilerbase
 **************************************************/ 
bool downloadFile(String subPath)
{
  bool result = false;
  String url = "http://" + conf.uhn + "/update.php?file="+subPath;
  int nPos = subPath.lastIndexOf("/");
  //save file to temp. name (to be sure, that complete update has been uploaded):
  String fname = subPath.substring(nPos)+"$";
  DEBUG_OUT.printf("[downloadFile] downloading \"%s\" to \"%s\"\n", subPath.c_str(), fname.c_str());

  http.setUserAgent(F("wifiOiler"));
  http.begin(client, url);
  // start connection and send HTTP header
  int httpCode = http.GET();
  DEBUG_OUT.printf("[downloadFile] GET return code: %d\n", httpCode);
  
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    //DEBUG_OUT.printf("[downloadFile] open file for writing %s\n", fname.c_str());
    
    // file found at server
    if (httpCode == HTTP_CODE_OK) {

      // get length of document (is -1 when Server sends no Content-Length header)
      uint32_t len = http.getSize();
      updFileSizeTotal = len;
      updSizeUploaded = 0;

      //DEBUG_OUT.printf("[downloadFile] file size is %d\n", updFileSizeTotal);

      // create buffer for read
      uint8_t buff[512] = { 0 };

      // get tcp stream
      WiFiClient * stream = http.getStreamPtr();

      outFile = _FILESYS.open(fname, "w");
      if (outFile)
      {
        // read all data from server
        while (http.connected() && (len > 0 || len == -1)) {
          
          // get available data size
          size_t size = stream->available();
          if(size) {
            myLedx.on(LED_GRUEN);
            // read up to (buffersize) byte
            int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
            outFile.write(buff, c);
            myLedx.off();
            updSizeUploaded += c;
            if(len > 0) {
              len -= c;
            }
          }
          webServer.handleClient(); // allow action while loading...
        }
        //DEBUG_OUT.println(F("[downloadFile] connection closed or file end - closing file..."));
        outFile.close();
        result = true;
      } else {
        DEBUG_OUT.println(F("Fehler beim Öffnen der Datei zum Schreiben!"));
      }
    }
    else if (httpCode <= 0)
      DEBUG_OUT.printf("[downloadFile] http.Get() failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();  
  return result;
}

/**********************************************************************
 * Helper function for handleUpdate()
 * try to download all files in updateFiles with state != _UF_UPLOADOK
 * returns false, if there was at least one error
 * _UF_INITIAL      0
 * _UF_UPLOADOK     1
 * _UF_UPLOAD_ERROR 2
 **********************************************************************/ 
bool handleUpdateFiles(void){
  bool result = true;
  for (uint8_t i = 0; i < updateFiles.size(); i++){
    if (updateFiles[i].state != _UF_UPLOADOK) {
      updateMessage += updateFiles[i].fileName; 
      webServer.handleClient();
  
      if (downloadFile(updateFiles[i].fileName)){   // wie mit Fehlern umgehen?
        updateFiles[i].state = _UF_UPLOADOK;
        DEBUG_OUT.print(F("[handleUpdate] download finshed:"));
        DEBUG_OUT.println(updateFiles[i].fileName);
        updateMessage += " (OK)<br>"; 
        webServer.handleClient();
      } else {
        updateFiles[i].state = _UF_UPLOAD_ERROR;
        DEBUG_OUT.println(F("[handleUpdate] error downloading file"));
        updateMessage += " (ERROR)<br>"; 
        webServer.handleClient();
        result = false;
      }
    }
  }  
  return result;
}

/**********************************************************************
 * Helper function for handleUpdate()
 * Renaming files after succesful upload
 * 1. if old exist: delete
 * 2. original to .old
 * 3. temp to new original
 **********************************************************************/ 
bool renameUpdateFiles(void){
  bool result = true;
  for (uint8_t i = 0; i < updateFiles.size() && result; i++){
    int16_t nPos = updateFiles[i].fileName.lastIndexOf("/");
    String fname = updateFiles[i].fileName.substring(nPos);
    DEBUG_OUT.print("[renameUpdateFiles] fname = ");
    DEBUG_OUT.println(fname);
    
    if (_FILESYS.exists(fname+".old"))
      result = (_FILESYS.remove(fname+".old"));

    if (!result){
      DEBUG_OUT.print(F("[renameUpdateFiles] ERROR removing "));
      DEBUG_OUT.println(fname+".old");
    } else {
      if (_FILESYS.exists(fname)) result = _FILESYS.rename(fname, fname+".old");
      if (!result){
        DEBUG_OUT.printf("[renameUpdateFiles] ERROR renaming file '%s' to '%s.old'\n", fname.c_str(), fname.c_str());
      } else {
        result = _FILESYS.rename(fname+"$", fname);
        if (!result)
          DEBUG_OUT.printf("[renameUpdateFiles] ERROR renaming file '%s$' to '%s'\n", fname.c_str(), fname.c_str());
      }
    }
  }
  return result;
}

/**********************************************************************
 * Helper function for handleUpdate()
 * Delete files after unsuccessful download attempts
 **********************************************************************/ 
uint8_t deleteUpdateFiles(void){
  uint8_t delErrorCount = 0;
  for (uint8_t i = 0; i < updateFiles.size(); i++){
    int16_t nPos = updateFiles[i].fileName.lastIndexOf("/");
    String fname = "/" + updateFiles[i].fileName.substring(nPos) + "$";
    DEBUG_OUT.print("[deleteUpdateFiles] delete ");
    DEBUG_OUT.print(fname);
    if (_FILESYS.exists(fname))
      if (!_FILESYS.remove(fname))
        delErrorCount++;
  }
  return (delErrorCount);
}

/***************************************************
 * Update vom Web-Server (files and firmware)
 * using globally defined:
 * 
 *  #define UPD_NOT_ACTIVE       0
 *  #define UPD_UPLOADING_FILES  1
 *  #define UPD_UPDATE_ENDED     2
 *  #define UPD_MUST_REBOOT      3
 *  #define UPD_REBOOT_NOW       4
 *  #define UPD_ERROR            5
 *  String updateMessage
 *  uint8_t updateResult
 *  uint16_t updFileSizeTotal
 *  uint16_t updSizeUploaded
 *  bool updError
 */
void handleUpdate(void)
{
  uint32_t wait;
  if (webServer.hasArg(F("update")))
  {
    updateMessage = "";
    updError = false;
    updateResult = UPD_NOT_ACTIVE;
  
    //erst mal sollte gecheckt werden, ob der Server überhaupt erreichbar ist...
    DEBUG_OUT.print(F("[handleUpdate] trying to connect to "));
    DEBUG_OUT.println(conf.uhn);
    if (!isServerAvailable())
    {
      return webServer.send( 500, TEXT_HTML, F("Update Server nicht erreichbar!"));
    }

    // Update Infos vom Server besorgen:
    DEBUG_OUT.print(F("[handleUpdate] trying to retrieve update info from "));
    DEBUG_OUT.println(conf.uhn);

    if (!getUpdateInfo())
    {
      return webServer.send( 500, TEXT_HTML, F("Server meldet: kein Update vorhanden!"));
    }
    else
    {
      updateResult = UPD_UPLOADING_FILES;
      webServer.send( 200, TEXT_HTML, updateMessage );
      webServer.handleClient();
//      wait = millis();
//      while (wait + 200 > millis()) webServer.handleClient();

      wait = 0;
      do { wait++; } while (!handleUpdateFiles() && wait <= 3);  // download files (3 attempts)
      updError = (wait > 3);
      if (updError) {
        deleteUpdateFiles();  // wenn nicht alle Dateien fehlerfrei hochgeladen wurden, wird update abgebrochen...
        updateResult = UPD_ERROR;
      } else {
        // bringt nichts:
        // updateMessage += "<br>renaming files...<br>";
        // wait = millis();
        // while (wait + 200 > millis()) webServer.handleClient();
        webServer.handleClient();
        renameUpdateFiles();
        updateResult = checkforUpdate(true) ? UPD_MUST_REBOOT : UPD_UPDATE_ENDED;
      }
      updateFiles.clear();
    }
  }
  else if (webServer.hasArg(F("result")))
  {
    switch (updateResult) {
      case UPD_NOT_ACTIVE:
        webServer.send( 200, TEXT_HTML, "NOUPDATE" );
        DEBUG_OUT.println(F("[handleUpdate(result)] just send 'NOUPDATE'"));
        break;
      case UPD_UPLOADING_FILES:
        webServer.send( 200, TEXT_HTML, updateMessage + "<br>( " + updSizeUploaded + " / " + updFileSizeTotal + " )<br>");
        //DEBUG_OUT.print(F("[handleUpdate(result)] just send 200 and "));
        //DEBUG_OUT.println(updateMessage);
        break;
      case UPD_UPDATE_ENDED:
        webServer.send( 200, TEXT_HTML, "UPDATEOK:" + updateMessage);
        DEBUG_OUT.println(F("[handleUpdate(result)] just send 'UPD_UPDATE_ENDED'"));
        updateMessage="";
        updateResult=UPD_NOT_ACTIVE;
        break;
      case UPD_MUST_REBOOT:
        webServer.send( 200, TEXT_HTML, "REBOOT:" + updateMessage );
        DEBUG_OUT.println(F("[handleUpdate(result)] just send 'UPD_MUST_REBOOT'"));
        DEBUG_OUT.println(F("[handleUpdate] set updateResult to UPD_REBOOT_NOW"));
        updateResult = UPD_REBOOT_NOW;  // eigentlicher Update in main loop()
        break;
      case UPD_ERROR:
        webServer.send( 200, TEXT_HTML, "UPDERROR:" + updateMessage );
        updateMessage="";
        updateResult=UPD_NOT_ACTIVE;
        break;
    }
  }
  else
  {
    if (updateResult != UPD_NOT_ACTIVE) return; // verhindern, dass update parallel nochmal aufgerufen wird
    else handleFileRead("/update.htm");
  }
}
