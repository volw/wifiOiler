const char PROGMEM * wifiFile = "/wifi.ini";
const IPAddress myIP(192, 168, 4, 1);

/*************************************
 * Lesen der WiFi Konfigurationsdaten
 *************************************/
uint8_t readWifiData(void) {
  String line;
  uint8_t pos;
  String key, value;
  uint8_t counter = 0;

  // ein GVwifiMulti.cleanAPlist() ist nicht nötig, da in der Lib geprüft wird,
  // ob wifi schon in der Liste vorhanden ist, bevor es hinzugefügt wird...
  // nachdem eine Verbindung hergestellt wurde, kann die Liste natürlich geleert werden 
  // (Speicherplatz/Sicherheit etc.)
  GVfsUploadFile = _FILESYS.open(wifiFile, "r");
  while(GVfsUploadFile.available()) {
    line = GVfsUploadFile.readStringUntil('\n');
    line.trim();

    if ((line.length() > 5) && !line.startsWith("//")) {
      if ((pos = line.indexOf("=")) > 0) {
        key = String(line.substring(0,pos));
        value = String(line.substring(pos+1));
        key.trim(); value.trim();
        DEBUG_OUT.print("Add wifi to wifiMulti: ");
        DEBUG_OUT.println(key.c_str());
        GVwifiMulti.addAP(key.c_str(), value.c_str());
        counter++;
      }
    }
  }
  GVfsUploadFile.close();
  return counter; // Anzahl wifi configs zur�ckgeben...
}

/*****************************************************************
 * setup WiFi
 * used, but not defined here:
 * -     myIP : IP-Adresse des Access Points
 * - GVoilerConf.apn : SSID von diesem Hotspot
 * - GVoilerConf.app : Passwort, um sich mit diesem Hotspot zu verbinden.
 * Versucht, mit einem bekanntem Wifi Netz zu verbinden - ist keins da oder
 * schlägt die Verbindung fehl, wird ein Access point errichtet...
 */
bool setupWiFi(void) {
  bool connected = false;
  
  DEBUG_OUT.println(F("Versuche, mit bekanntem Wifi Netz zu verbinden..."));
  WiFi.disconnect();  // angeblich werden vorher gemerkte Zugangsdaten damit gelöscht...

  // Daten konfigurierter Netze in LittleFS: /wifi.ini
  if (readWifiData() > 0)
  {
    WiFi.mode(WIFI_STA);   // Only station Mode
    DEBUG_OUT.print(F("Connecting Wifi "));
    GVmyDisplay.PrintMessage("Suche WLAN");

    byte count = 0;
    while ((GVwifiMulti.run() != WL_CONNECTED) && (count < 10)) {
      delay(500);
      DEBUG_OUT.print(".");
      GVmyDisplay.MessageAdd(".");
      count++;
    }
  
    if (GVwifiMulti.run() == WL_CONNECTED) {
      GVmyDisplay.PrintMessage("Connected:");
      GVmyDisplay.MessageAdd(WiFi.SSID());
      GVmyDisplay.MessageAdd("\n");
      GVmyDisplay.MessageAdd(WiFi.localIP().toString(), 1500);
      
      DEBUG_OUT.print(F("\nWiFi connected to '"));
      DEBUG_OUT.print(WiFi.SSID());
      DEBUG_OUT.print(F("' - IP address: "));
      DEBUG_OUT.println(WiFi.localIP());
      //GVmyLedx.start LED_WIFI_CONNECT_SUCCESS;
      GVmyDisplay.PrintWlanState(WIFI_WLAN);
      connected = true;
    } 
    else 
    {
      GVmyDisplay.MessageAdd("\nFailed!",1500);
      DEBUG_OUT.println(F("\nnot connected to any WiFi"));
      //GVmyLedx.start LED_WIFI_CONNECT_FAILED;
      WiFi.disconnect();
    }
    GVwifiMulti.cleanAPlist();
  }
  else DEBUG_OUT.println(F("Keine WiFi Konfiguration(en) gefunden"));

  if (!connected)
  {
    GVmyDisplay.PrintMessage("Starting\nAcc. Point\n");
    GVmyDisplay.MessageAdd(GVoilerConf.apn, 1500);
    WiFi.mode(WIFI_AP);   // only Access Point
    WiFi.softAPConfig(myIP, myIP, IPAddress(255, 255, 255, 0));
    DEBUG_OUT.print(F("Setting up access point '"));
    DEBUG_OUT.print(GVoilerConf.apn);
    DEBUG_OUT.print(F("' - password '"));
    DEBUG_OUT.print(GVoilerConf.app);
    DEBUG_OUT.println("'");
    
    if (WiFi.softAP(GVoilerConf.apn, GVoilerConf.app))
    {
      delay(500); // Without delay I've seen the IP address blank
      DEBUG_OUT.print(F("My IP address: "));
      DEBUG_OUT.println(WiFi.softAPIP());
      GVmyDisplay.PrintWlanState(WIFI_ACCESSPOINT);
      connected = true;
    }
    else 
    {
      DEBUG_OUT.println(F("Failed setting up access point!!"));
      GVmyDisplay.PrintWlanState(WIFI_INACTIVE);
    }
  }
  return connected;
}

/*****************************************************************
 * setup mDNS
 *****************************************************************/
void setupMDNS(void) {
  if (!MDNS.begin(GVoilerConf.apn)) {
    DEBUG_OUT.println(F("Error setting up mDNS responder!..."));
    while(1) delay(1000); // trigger watch dog
  }
  else 
  {
    DEBUG_OUT.print(F("mDNS responder started, please connect to \"http://"));
    DEBUG_OUT.print(GVoilerConf.apn);
    DEBUG_OUT.println(F(".local\""));
  }
}

/*****************************************************************
 * Toggle WiFi
 *****************************************************************/
void toggleWiFi(void)
{
  if (GVwifiSleeping)
  {
    DEBUG_OUT.println(F("starting WiFi ..."));
    WiFi.forceSleepWake(); // Wifi on
    delay(100);

    GVmyLedx.on(LED_GRUEN); // LED an waehrend Initialisierung
    bool wifiConnected = setupWiFi();  // dauert bis zu 10s
  
    if (wifiConnected) {
      // if DNSServer is started with "*" for domain name, it will reply with
      // provided IP to all DNS request ("captive Portal")
      GVdnsServer.start(DNS_PORT, "*", myIP);
      delay(100);
  
      setupMDNS();
      delay(100);
      GVmyLedx.start LED_WIFI_CONNECT_SUCCESS;
      GVwifiSleeping = false;
      GVswitchOnMillis = millis();
    } else {
      GVmyLedx.start LED_WIFI_CONNECT_FAILED;
      //TODO: error handling, possibly undefined wifi state if !wifiConnected
    }
    GVmyLedx.delay();
  }
  else
  {
    DEBUG_OUT.println(F("stopping WiFi..."));
    MDNS.close();
    WiFi.disconnect();
    delay(100);
    WiFi.forceSleepBegin(); // Wifi off
    GVwifiSleeping = true;
    GVmyLedx.start LED_WIFI_CONNECT_FAILED;
    GVmyDisplay.PrintWlanState(WIFI_INACTIVE);
  }
}

/***********************************************
 * checkWiFi: WiFi wird ausgeschaltet, wenn sich
 * die Kiste bewegt.
 ***********************************************/
void checkWiFi(void)
{
  if ((GVlastWiFiCheck + 3000) < millis()) {
    if ((GVoilerConf.wts * 1000) < millis() && isMoving() && ((GVswitchOnMillis + 30000) < millis())) 
      toggleWiFi(); // WiFi ggf. ausschalten
    else 
      if (!GVmyLedx.isActive() && !GVmaintenanceMode) GVmyLedx.start LED_SHOW_WIFI_ON;  // nur nicht GVmaintenanceMode, sonst zu viel Geblinke...
    GVlastWiFiCheck = millis();
  }  
}
