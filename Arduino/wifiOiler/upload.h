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

#define HEADER_LENGTH 195
const String boundary = "wifiOiler-dea88064e2e3320a";

/*************************************************
 * Check, ob konfigurierter Upload-/Update server 
 * erreichbar ist.
 *************************************************/ 
bool isServerAvailable(void) {
  String cURL = "http://"+GVoilerConf.uhn+":"+String(GVoilerConf.uhp);
  GVhttp.begin(GVwifiClient, cURL);
  int httpCode = GVhttp.GET();
  GVhttp.end();
  
  DEBUG_OUT.print(F("[isServerAvailable] GET, URL:"));
  DEBUG_OUT.println(cURL);
  IPAddress remoteHostIP;
  if (WiFi.hostByName(GVoilerConf.uhn.c_str(), remoteHostIP, 5000))
  {
    DEBUG_OUT.print(F("[isServerAvailable] IP-Adresse von '"));
    DEBUG_OUT.print(GVoilerConf.uhn.c_str());
    DEBUG_OUT.print(F("' ist "));
    DEBUG_OUT.println(remoteHostIP.toString());
  }
  if (httpCode <= 0){
    DEBUG_OUT.print(F("[isServerAvailable] GET() failed, error: "));
    DEBUG_OUT.println(GVhttp.errorToString(httpCode));
  }
  return (httpCode==200);
}

/*************************************************
 * Ask oilerbase, if specific file exists
 * Then the uploaded file has arrived safely ;-)
 * oierbase should return http 200
 *************************************************/ 
bool isFileThere(String fname) {
  //GVhttp.setUserAgent(F(HTTP_USER_AGENT));
  GVhttp.begin(GVwifiClient ,"http://"+GVoilerConf.uhn+"/fileexists.php?filename="+fname);
  int httpCode = GVhttp.GET();
  DEBUG_OUT.print(F("[isFileThere] httpCode = "));
  DEBUG_OUT.println(httpCode);
  GVhttp.end();
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
      DEBUG_OUT.println(F("[sendFile] Starte Übertragung der Datei..."));
      //sendContent(F("POST /upload.php HTTP/1.1"));
      GVwifiClient.println("POST " + GVoilerConf.url + " HTTP/1.1");
      GVwifiClient.println("Host: " + GVoilerConf.uhn);
      GVwifiClient.println(F("User-Agent: wifiOiler/0.1"));    // wird auf Serverseite abgefragt - ggf. dort anpassen
      GVwifiClient.println(F("Accept: */*"));
      GVwifiClient.println("Content-Length:" + String(HEADER_LENGTH + GVoutFile.size()+fname.length()));
      GVwifiClient.println("Content-Type: multipart/form-data; boundary=" + boundary);
      GVwifiClient.println("\r\n--" +  boundary);
      GVwifiClient.println("Content-Disposition: form-data; name=\"userfile\"; filename=\"" + fname + "\"");
      GVwifiClient.println(F("Content-Type: application/octet-stream\r\n"));
      DEBUG_OUT.println(F("[sendFile] >>> sending file >>>"));
      GVwifiClient.write(GVoutFile);
      GVwifiClient.println("\r\n--" + boundary + "--\r\n");

      // don't wait for any response - we check existance with get() Call...
      // while (GVwifiClient.available()) GVwifiClient.read();
      GVwifiClient.stop();
      DEBUG_OUT.println(F("[sendFile] Übertragung beendet"));
      success = isFileThere(fname);
      if (success) {
        // Datei ist angekommen und kann gelöscht werden:
        _FILESYS.remove("/"+fname);
      }
    }
    else 
    {
      DEBUG_OUT.print(F("Fehler beim Öffnen der Verbindung zu: "));
      DEBUG_OUT.println(GVoilerConf.uhn);
    }
    GVoutFile.close();
  }
  else 
  {
    DEBUG_OUT.print(F("Fehler beim Öffnen der Datei: "));
    DEBUG_OUT.println(fname);
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
      DEBUG_OUT.println(F("searching track files:"));
      uploadResponse += F("searching track files...\n"); GVwebServer.handleClient();
      // Track file names: yyyymmdd-hhmm.dat
      Dir dir = _FILESYS.openDir("/20");
      
      while (dir.next()) {
        String fname = dir.fileName();
        DEBUG_OUT.print(fname);
        #ifdef _SPIFFS_
          //SPIFFS: beim SPIFFS ist die Länge 18 (nicht 17), da der leitende "/" mitgezählt wird.
          if (fname.endsWith(".dat") && (fname.length() == 17))  // Track-File (incl. einleitendem "/")
        #else
          //LittleFS:
          if (fname.endsWith(".dat") && (fname.length() == 17))
        #endif
        {
          DEBUG_OUT.print(F(" is a track file - will upload..."));
          uploadResponse += "...uploading " + fname + "..."; GVwebServer.handleClient();
          GVmyLedx.on(LED_GRUEN);
          if (sendFile(fname))
          {
            GVmyLedx.start LED_TRACK_UPLOAD_SUCCESS;
            uploadOK++;
            DEBUG_OUT.println(F("OK"));
            uploadResponse += F("OK\n"); GVwebServer.handleClient();
          }
          else 
          {
            GVmyLedx.start LED_TRACK_UPLOAD_FAILED;
            uploadFailed++;
            DEBUG_OUT.println(F("FAILED"));
            uploadResponse += F("FAILED\n"); GVwebServer.handleClient();
          }
          GVmyLedx.delay();
        }
        else
        {
          DEBUG_OUT.println(F(" is NO track file..."));
        }
      }
      if ((uploadOK + uploadFailed) == 0)
      {
        uploadResponse = F("\nKeine Dateien zum Hochladen gefunden\n-END-"); GVwebServer.handleClient();
      }
      else
      {
        uploadResponse += "\nUpload beendet\nTracks hochgeladen: " + String(uploadOK);
        uploadResponse += "\nUpload Fehler: "+String(uploadFailed) + "\n-END-";
        GVwebServer.handleClient();
        if (uploadOK > 0) checkFilesystemSpace();
      }
    }
    else handleMessage(F("Upload Server not available!"));
  }
}
