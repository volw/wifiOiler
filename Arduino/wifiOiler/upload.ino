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

/*************************************************
 * Check, ob konfigurierter Upload-/Update server 
 * erreichbar ist.
 *************************************************/ 
bool isServerAvailable(void) {
  String cURL = getOilerbaseURL();
//  String cURL = "http://oilerbase.fritz.box/oilerbase.php";
  GVhttp.setAuthorization(GVoilerConf.bac.c_str());
  GVhttp.begin(GVwifiClient, cURL);
  int httpCode = GVhttp.GET();
  GVhttp.end();
  
  DEBUG_OUT.printf(PSTR(MSG_DBG_CHECK_URL_START), cURL.c_str());
  IPAddress remoteHostIP;
  if (WiFi.hostByName(GVoilerConf.uhn.c_str(), remoteHostIP, 5000))
  {
    DEBUG_OUT.printf(PSTR(MSG_DBG_CHECK_URL_SERVER_IP), GVoilerConf.uhn.c_str(), remoteHostIP.toString().c_str());
  }
  if (httpCode <= 0){
    DEBUG_OUT.printf(PSTR(MSG_DBG_CHECK_URL_ERROR), GVhttp.errorToString(httpCode).c_str());
  }
  return (httpCode==200);
}

/*************************************************
 * Ask oilerbase, if specific file exists
 * Then the uploaded file has arrived safely ;-)
 * oierbase should return http 200
 *************************************************/ 
bool isFileThere(String fname) { 
  uint8_t numTrials = 3;  // check more than once because of timing problems with multiple (bigger?) files
  int httpCode = 0;
  while (httpCode != 200 && numTrials > 0) {
    GVhttp.begin(GVwifiClient ,getOilerbaseURL() + "?filename="+fname);
    httpCode = GVhttp.GET();
    DEBUG_OUT.printf(PSTR(MSG_DBG_FILECHECK_RESULT), httpCode);
    GVhttp.end();
    numTrials--;
  }
  return (httpCode == 200);   // 200, wenn Datei vorhanden
}

/*************************************************
 * Post/Upload file to oilerbase
 * Return true when no error
 *************************************************/ 
bool sendFile(String fname) {
  bool success = false;
  
  if (fname.startsWith("/")) fname = fname.substring(1);
  GVoutFile = _FILESYS.open("/"+fname, "r");
  if (GVoutFile) {
    if (GVwifiClient.connect(GVoilerConf.uhn, GVoilerConf.uhp)) {
      DEBUG_OUT.println(F(MSG_DBG_SEND_FILE_START));
      //------------------- Header -------------------------------------
      GVwifiClient.println("POST " + GVoilerConf.url + " HTTP/1.1");
      GVwifiClient.println("Host: " + GVoilerConf.uhn+":"+GVoilerConf.uhp);
      GVwifiClient.println(F("Accept: */*"));
      GVwifiClient.printf(PSTR("User-Agent: %s/%s\r\n"), HTTP_USER_AGENT, VERSION);    // user agent wird auf Serverseite abgefragt - ggf. dort anpassen
      if (GVoilerConf.bac != "") {
        // adding authorization header for basic authentication (s. #define in wifiOiler.h for auth. string)
        GVwifiClient.printf(PSTR("Authorization: Basic %s\r\n"), GVoilerConf.bac.c_str());
      }
      GVwifiClient.printf(PSTR("Content-Length: %d\r\n"), HEADER_LENGTH + GVoutFile.size() );
      GVwifiClient.println("Content-Type: multipart/form-data; boundary=" + boundary);
      GVwifiClient.println();
      //------------------- End of Header ------------------------------

      /*  44 */ GVwifiClient.println("--" +  boundary);
      /*  79 */ GVwifiClient.println("Content-Disposition: form-data; name=\"userfile\"; filename=\"" + fname + "\"");
      /*  40 */ GVwifiClient.println("Content-Type: application/octet-stream");
      /*   2 */ GVwifiClient.println();
      /*  -- */ GVwifiClient.write(GVoutFile);
      /*   2 */ GVwifiClient.println();
      /*  46 */ GVwifiClient.println("--" + boundary + "--");

      // don't wait for any response - we check existance with get() Call...
      // while (GVwifiClient.available()) DEBUG_OUT.print(GVwifiClient.read());
      GVwifiClient.stop();
      DEBUG_OUT.println(F(MSG_DBG_SEND_FILE_COMPLETED));
      success = isFileThere(fname);
      if (success) {
        // Datei ist angekommen und kann gelöscht werden:
        _FILESYS.remove("/"+fname);
      }
    }
    else 
    {
      DEBUG_OUT.printf(PSTR(MSG_DBG_SEND_FILE_CONNECT_ERROR), GVoilerConf.uhn.c_str(), GVoilerConf.uhp);
    }
    GVoutFile.close();
  }
  else 
  {
    DEBUG_OUT.printf(PSTR(MSG_DBG_SEND_FILE_FOPEN_ERROR), fname.c_str());
  }
  return success;
}

// Meine Messung nach https://stackoverflow.com/questions/8659808/how-does-http-file-upload-work
// Die gemessene Länge ging nie korrekt bis aufs byte auf - habe den Eindruck, dass das Ende der Datei
// nur irgendwo innerhalb der boundary landen muss - sonst Fehler.
//
// POST / HTTP/1.1
// Host: localhost:8000
// User-Agent: curl/7.52.1
// Accept: */*
// Content-Length: 230
// Expect: 100-continue
// Content-Type: multipart/form-data; boundary=------------------------dea88064e2e3320a
//
// Attention: two extra '-':
// --------------------------dea88064e2e3320a
// Content-Disposition: form-data; name="userfile"; filename="test.bin"
// Content-Type: application/octet-stream
// 
// aaabbbcccddd
// eeefffggghhh
// --------------------------dea88064e2e3320a--
//



/***************************************************
 * Upload von Track files
 */
String uploadResponse;  // can't be local

void handleUpload(void)
{
  if (GVwebServer.hasArg(F("result")))
  {
    GVwebServer.send(200, TEXT_PLAIN, uploadResponse);
    /* das "-END-" wird per javascript am client abgefragt und ist das Kennzeichen dafür, dass nichts mehr kommt...
     * gleichzeitig wird der uploadResponse String verkleinert, 
     * um wieder Speicherplatz freizugeben (falls das überhaupt dann passiert)... */
    if (uploadResponse.endsWith("-END-")) uploadResponse = "-END-";
    GVwebServer.handleClient();
  }
  else
  {
    //erst mal sollte gecheckt werden, ob der Server überhaupt erreichbar ist...
    uploadResponse = "";
    if (isServerAvailable())
    {
      // Warteseite anzeigen:
      handleFileRead("/upload.htm");
      uint32_t wait = millis();
      // 1s warten und webServer damit Zeit für Aktion zu geben
      while (wait + 1000 > millis()) GVwebServer.handleClient();
      
      uint8_t uploadOK = 0;
      uint8_t uploadFailed = 0;
      DEBUG_OUT.println(F(MSG_DBG_TRACK_UPLOAD_START));
      uploadResponse += F("searching track files...\n"); GVwebServer.handleClient();
      // Track file names: yyyymmdd-hhmm.dat
      Dir dir = _FILESYS.openDir("/2");  // hier Jahr-3000 Problematik ;-)
      
      while (dir.next()) {
        String fname = dir.fileName();
        if (fname.endsWith(".dat") && (fname.length() == 17))
        {
          DEBUG_OUT.printf(PSTR(MSG_DBG_TRACK_FILE_FOUND_YES), fname.c_str());
          uploadResponse += "...uploading " + fname + "..."; GVwebServer.handleClient();
          GVmyLedx.on(LED_GRUEN);
          if (sendFile(fname))
          {
            GVmyLedx.start LED_TRACK_UPLOAD_SUCCESS;
            uploadOK++;
            DEBUG_OUT.println(F(MSG_DBG_TRACK_UPLOAD_OK));
            uploadResponse += F("OK\n"); GVwebServer.handleClient();
          }
          else 
          {
            GVmyLedx.start LED_TRACK_UPLOAD_FAILED;
            uploadFailed++;
            DEBUG_OUT.println(F(MSG_DBG_TRACK_UPLOAD_FAILED));
            uploadResponse += F("FAILED\n"); GVwebServer.handleClient();
          }
          GVmyLedx.delay();
        }
        else
        {
          DEBUG_OUT.printf(PSTR(MSG_DBG_TRACK_FILE_FOUND_NO), fname.c_str());
        }
      }
      if ((uploadOK + uploadFailed) == 0)
      {
        uploadResponse = F(MSG_HTTP_NO_TRACKS_FOUND); GVwebServer.handleClient();
      }
      else
      {
        uploadResponse += "\nUpload beendet\nTracks hochgeladen: " + String(uploadOK);
        uploadResponse += "\nUpload Fehler: "+String(uploadFailed) + "\n-END-";
        GVwebServer.handleClient();
        if (uploadOK > 0) checkFilesystemSpace();
      }
    }
    else handleMessage(F(MSG_DBG_UPLOAD_SERVER_ERROR));
  }
}
