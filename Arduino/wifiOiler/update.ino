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
#define BUFFER_SIZE 1024    // adjust, if not enough RAM at runtime

/**************************************************************
 * Callback function for stream update
 **************************************************************/
void updateCallback(size_t currSize, size_t totalSize) {
  static boolean lGreen = false;
  lGreen = !lGreen;
  GVmyLedx.on(lGreen ? LED_GRUEN : LED_ROT);
}

/**************************************************************
 * check for new binary in file system (file name see <GVoilerConf.ffn>)
 * when found: update & restart
 **************************************************************/
bool checkforUpdate(bool justCheck, bool reboot) {
  bool update = false;

  String FirmwareFilename = GVoilerConf.ffn;
  if (!FirmwareFilename.startsWith("/")) FirmwareFilename = "/" + GVoilerConf.ffn;
  DEBUG_OUT.print(F(MSG_DBG_UPDATE_INIT));
  DEBUG_OUT.println(FirmwareFilename);
  if (justCheck) return(_FILESYS.exists(FirmwareFilename));
  
  // ist justCheck == false, wird Firmware eingespielt
  if (_FILESYS.exists(FirmwareFilename))
  {
    DEBUG_OUT.println(F(MSG_DBG_UPDATE_START));
    GVmyDisplay.PrintMessage("Firmware\nfound:\nupdating..");
    File file = _FILESYS.open(FirmwareFilename, "r");

    if (file) {
      Update.onProgress(updateCallback);
      if (!Update.begin(file.size(), U_FLASH)) {
        Update.printError(DEBUG_OUT);
        DEBUG_OUT.println(F(MSG_DBG_UPDATE_ERROR));
      } else {
        GVmyLedx.on(LED_ROT);
        Update.writeStream(file);
        update = Update.end();
        if (update){
          DEBUG_OUT.println(F(MSG_DBG_UPDATE_SUCCESS));
        } else {
          DEBUG_OUT.print(F(MSG_DBG_UPDATE_GETERROR));
          DEBUG_OUT.println(Update.getError());
        }
      }
      file.close();      
      GVmyLedx.off();
    }

    DEBUG_OUT.print(F(MSG_DBG_REMOVING_FIRMWARE_FILE));
    DEBUG_OUT.println(FirmwareFilename);
    if (!_FILESYS.remove(FirmwareFilename)) 
      DEBUG_OUT.println(F(MSG_DBG_ERROR_DEL_FIRMWARE_FILE));

    if (update && reboot)
    {
      GVmyDisplay.PrintMessage(F("Reboot..."));
      DEBUG_OUT.println(F(MSG_DBG_BOOT_AFTER_UPDATE));
      delay(10);
      oilerReboot();
    }
    else GVmyDisplay.MessageOff();
  }
  else 
  {
    DEBUG_OUT.println(MSG_DBG_NO_FIRMWARE_FILE);
  }
  return update;
}

/***********************************************************
 * Get update info from oilerbase (when available)
 * BOARD_TYPE, VERSION in URL for php script on oilerbase
 * expecting result: list of update files (separated by ';')
 ***********************************************************/ 
bool getUpdateInfo(void)
{
  GVupdateFiles.clear();  // empty vector
  String url = getOilerbaseURL() + "?board="+BOARD_TYPE+"&version="+VERSION;
  String result = "";

  //GVhttp.setUserAgent(F(HTTP_USER_AGENT));
  GVhttp.setAuthorization(GVoilerConf.bac.c_str());
  GVhttp.begin(GVwifiClient, url);

  int httpCode = GVhttp.GET();
  DEBUG_OUT.printf(PSTR(MSG_DBG_UPDATE_SERVER_URL), url.c_str());
  DEBUG_OUT.printf(PSTR(MSG_DBG_UPD_INFO_GET_RET_CODE), httpCode);
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    result = GVhttp.getString();

    int16_t nPos = 0, nStart = 0; // DO NOT USE UNSIGNED-TYPE HERE (can be < 0)
    if (httpCode == 200 && result.length() > 0) {
      while (result.charAt(nPos) < 32) nPos++;
      if (nPos > 0) result = result.substring(nPos);
  
      nStart = nPos = result.length()-1;
      while (result.charAt(nPos) < 32) nPos--;
      if (nPos < nStart) result = result.substring(0, nPos+1);
      
      DEBUG_OUT.print(F(MSG_DBG_UPD_INFO_GET_RET_TEXT));
      DEBUG_OUT.println(result);

      while ((nPos=result.indexOf(';')) > 0) {
        GVupdateFiles.push_back({result.substring(0, nPos),_UF_INITIAL});
        result = result.substring(nPos+1);
      }
      GVupdateFiles.push_back({result,_UF_INITIAL});
    }
  }
  else
  {
    DEBUG_OUT.printf(PSTR(MSG_DBG_UPD_INFO_GET_ERROR), GVhttp.errorToString(httpCode).c_str());
  }
  GVhttp.end(); 
  return (GVupdateFiles.size() > 0);
}

/**************************************************
 * Request file (subPath) from oilerbase
 **************************************************/ 
bool downloadFile(String subPath)
{
  bool result = false;
  String url = getOilerbaseURL() + "?file="+subPath;
  int nPos = subPath.lastIndexOf("/");
  //save file to temp. name (to be sure, that complete update has been uploaded):
  String fname = subPath.substring(nPos)+"$";
  DEBUG_OUT.printf(PSTR(MSG_DBG_UPD_DOWNLOAD_INFO), subPath.c_str(), fname.c_str());

  GVhttp.begin(GVwifiClient, url);
  // start connection and send HTTP header
  int httpCode = GVhttp.GET();
  DEBUG_OUT.printf(PSTR(MSG_DBG_UPD_DOWNLOAD_RET_CODE), httpCode);
  
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    
    // file found at server
    if (httpCode == HTTP_CODE_OK) {

      // get length of document (is -1 when Server sends no Content-Length header)
      int32_t fileLen = GVhttp.getSize();
      GVupdFileSizeTotal = fileLen;
      GVupdSizeUploaded = 0;

      //DEBUG_OUT.printf("[downloadFile] file size is %d\n", GVupdFileSizeTotal);

      // create buffer for read
      uint8_t buff[BUFFER_SIZE] = { 0 };

      // get tcp stream
      WiFiClient * stream = GVhttp.getStreamPtr();

      GVoutFile = _FILESYS.open(fname, "w");
      if (GVoutFile)
      {
        GVmyDisplay.PrintMessage("FileUpload\n");
        GVmyDisplay.MessageAdd(fname);

        // read all data from server
        while (GVhttp.connected() && (fileLen > 0 || fileLen == -1)) {
          
          // get available data size
          size_t size = stream->available();
          //DEBUG_OUT.printf("[downloadFile] stream size available: %d\n", size);
          if(size) {
            GVmyLedx.on(LED_GRUEN);
            // read up to (buffersize) byte
            int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
            GVoutFile.write(buff, c);
            GVmyLedx.off();
            GVupdSizeUploaded += c;
            if(fileLen > 0) {
              fileLen -= c;
            }
          }
          GVwebServer.handleClient(); // allow action while loading...
        }
        //DEBUG_OUT.println(F("[downloadFile] connection closed or file end - closing file..."));
        GVoutFile.close();
        GVmyLedx.start LED_FILE_UPLOAD_SUCCESS;
        GVmyDisplay.PrintMessage("Success");
        GVmyLedx.delay();
        result = true;
      } else {
        DEBUG_OUT.println(F(MSG_DBG_UPD_DOWNLOAD_OPEN_ERROR));
      }
    }
    else if (httpCode <= 0)
      DEBUG_OUT.printf(PSTR(MSG_DBG_UPD_DOWNLOAD_GET_ERROR), GVhttp.errorToString(httpCode).c_str());
  }
  GVhttp.end();  
  return result;
}

/**********************************************************************
 * Helper function for handleUpdate()
 * try to download all files in GVupdateFiles with state != _UF_UPLOADOK
 * returns false, if there was at least one error
 * _UF_INITIAL      0
 * _UF_UPLOADOK     1
 * _UF_UPLOAD_ERROR 2
 **********************************************************************/ 
bool handleUpdateFiles(void){
  bool result = true;
  for (uint8_t i = 0; i < GVupdateFiles.size(); i++){
    if (GVupdateFiles[i].state != _UF_UPLOADOK) {
      GVupdateMessage += GVupdateFiles[i].fileName; 
      GVwebServer.handleClient();
  
      if (downloadFile(GVupdateFiles[i].fileName)){
        GVupdateFiles[i].state = _UF_UPLOADOK;
        DEBUG_OUT.print(F(MSG_DBG_UPD_DOWNLOAD_COMPLETE));
        DEBUG_OUT.println(GVupdateFiles[i].fileName);
        GVupdateMessage += " (OK)<br>"; 
        GVwebServer.handleClient();
      } else {
        GVupdateFiles[i].state = _UF_UPLOAD_ERROR;
        DEBUG_OUT.println(F(MSG_DBG_UPD_DOWNLOAD_ERROR));
        GVupdateMessage += " (ERROR)<br>"; 
        GVwebServer.handleClient();
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
  for (uint8_t i = 0; i < GVupdateFiles.size() && result; i++){
    int16_t nPos = GVupdateFiles[i].fileName.lastIndexOf("/");
    String fname = GVupdateFiles[i].fileName.substring(nPos);
    DEBUG_OUT.print(MSG_DBG_RENAME_FILE_INIT);
    DEBUG_OUT.println(fname);
    
    if (_FILESYS.exists(fname+".old"))
      result = (_FILESYS.remove(fname+".old"));

    if (!result){
      DEBUG_OUT.print(F(MSG_DBG_OLD_FILE_REMOVE_ERROR));
      DEBUG_OUT.println(fname+".old");
    } else {
      if (_FILESYS.exists(fname)) result = _FILESYS.rename(fname, fname+".old");
      if (!result){
        DEBUG_OUT.printf(PSTR(MSG_DBG_OLD_FILE_RENAME_ERROR), fname.c_str(), fname.c_str());
      } else {
        result = _FILESYS.rename(fname+"$", fname);
        if (!result)
          DEBUG_OUT.printf(PSTR(MSG_DBG_TMP_FILE_RENAME_ERROR), fname.c_str(), fname.c_str());
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
  for (uint8_t i = 0; i < GVupdateFiles.size(); i++){
    int16_t nPos = GVupdateFiles[i].fileName.lastIndexOf("/");
    String fname = "/" + GVupdateFiles[i].fileName.substring(nPos) + "$";
    DEBUG_OUT.print(F(MSG_DBG_UPD_FILE_DELETE_INIT));
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
 *  String GVupdateMessage
 *  uint8_t GVupdateResult
 *  uint16_t GVupdFileSizeTotal
 *  uint16_t GVupdSizeUploaded
 *  bool GVupdError
 */
void handleUpdate(void)
{
  uint32_t wait;
  int httpResult;
  if (GVwebServer.hasArg(F("update")))
  {
    GVupdateMessage = "";
    GVupdError = false;
    GVupdateResult = UPD_NOT_ACTIVE;
  
    //erst mal sollte gecheckt werden, ob der Server überhaupt erreichbar ist...
    DEBUG_OUT.printf(PSTR(MSG_DBG_UPD_CONNECT_INIT));
   
    httpResult = isServerAvailable();
    if (httpResult == 401) {
      return GVwebServer.send( 500, TEXT_HTML, MSG_DBG_CHECK_URL_HTTP_ERR_401);
    } else if (httpResult != 200) {
      return GVwebServer.send( 500, TEXT_HTML, MSG_HTTP_UPD_SERVER_CONNECT_ERROR);
    }

    // Update Infos vom Server besorgen:
    DEBUG_OUT.print(F(MSG_DBG_GET_UPDATE_INFO));
    DEBUG_OUT.println(GVoilerConf.uhn);

    if (!getUpdateInfo())
    {
      return GVwebServer.send( 500, TEXT_HTML, F(MSG_HTTP_NO_UPDATE_AVAILABLE));
    }
    else
    {
      GVupdateResult = UPD_UPLOADING_FILES;
      GVwebServer.send( 200, TEXT_HTML, GVupdateMessage );
      GVwebServer.handleClient();

      wait = 0;
      do { wait++; } while (!handleUpdateFiles() && wait < 3);  // download files (3 attempts)
      GVupdError = (wait >= 3);
      if (GVupdError) {
        deleteUpdateFiles();  // wenn nicht alle Dateien fehlerfrei hochgeladen wurden, wird update abgebrochen...
        GVupdateResult = UPD_ERROR;
      } else {
        GVwebServer.handleClient();
        renameUpdateFiles();
        GVupdateResult = checkforUpdate(true) ? UPD_MUST_REBOOT : UPD_UPDATE_ENDED;
      }
      GVupdateFiles.clear();
    }
  }
  else if (GVwebServer.hasArg(F("result")))
  {
    switch (GVupdateResult) {
      case UPD_NOT_ACTIVE:
        GVwebServer.send( 200, TEXT_HTML, "NOUPDATE" );
        break;
      case UPD_UPLOADING_FILES:
        //send GVupdateMessage plus temporary progress (bar, % and totals):
        GVwebServer.send( 200, TEXT_HTML, GVupdateMessage + "<br><progress max='"+GVupdFileSizeTotal+"' value='"+GVupdSizeUploaded+"'></progress>&nbsp;"+((GVupdSizeUploaded * 100) / GVupdFileSizeTotal)+"%<br>"+ GVupdSizeUploaded + " / " + GVupdFileSizeTotal);
        break;
      case UPD_UPDATE_ENDED:
        GVwebServer.send( 200, TEXT_HTML, "UPDATEOK:" + GVupdateMessage);
        GVupdateMessage="";
        GVupdateResult=UPD_NOT_ACTIVE;
        break;
      case UPD_MUST_REBOOT:
        GVwebServer.send( 200, TEXT_HTML, "REBOOT:" + GVupdateMessage );
        GVupdateResult = UPD_REBOOT_NOW;  // eigentlicher Update in main loop()
        break;
      case UPD_ERROR:
        GVwebServer.send( 200, TEXT_HTML, "UPDERROR:" + GVupdateMessage );
        GVupdateMessage="";
        GVupdateResult=UPD_NOT_ACTIVE;
        break;
    }
  }
  else
  {
    if (GVupdateResult != UPD_NOT_ACTIVE) return; // verhindern, dass update parallel nochmal aufgerufen wird
    else handleFileRead("/update.htm");
  }
}
