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
        if (key.length() > 0) {
          DEBUG_OUT.print(F(MSG_DBG_ADD_WIFI_TO_MULTIWIFI));
          DEBUG_OUT.println(key.c_str());
          GVwifiMulti.addAP(key.c_str(), value.c_str());
          counter++;
        }
      }
    }
  }
  GVfsUploadFile.close();
  return counter; // Anzahl wifi configs zurückgeben...
}

/*****************************************************************
 * setup WiFi
 * Versucht, mit einem bekannten Wifi Netz zu verbinden - ist keins da oder
 * schlägt die Verbindung fehl, wird ein Access point errichtet...
 */
bool setupWiFi(void) {
  bool connected = false;
  
  DEBUG_OUT.println(F(MSG_DBG_START_SEARCHING_WIFI));
  GVwifiAPmode = false;
  WiFi.disconnect();  // angeblich werden vorher gemerkte Zugangsdaten damit gelöscht...
  WiFi.hostname(GVoilerConf.apn);

  // Daten konfigurierter Netze in LittleFS: /wifi.ini
  if (readWifiData() > 0)
  {
    //WiFi.mode(WIFI_STA);   // Only station Mode
    DEBUG_OUT.print(F(MSG_DBG_TRY_CONNECT_WIFI));
    GVmyDisplay.PrintMessage("Suche WLAN");

    byte count = 0;
    while ((GVwifiMulti.run() != WL_CONNECTED) && (count < 3)) { // scan timout = 5000, connect timeout = 5000
      DEBUG_OUT.print(".");
      GVmyDisplay.MessageAdd(".");
      count++;
    }
    DEBUG_OUT.println();
  
    if (GVwifiMulti.run() == WL_CONNECTED) {
      GVmyDisplay.PrintMessage("Connected:");
      GVmyDisplay.MessageAdd(WiFi.SSID());
      GVmyDisplay.MessageAdd("\n");
      GVmyDisplay.MessageAdd(WiFi.localIP().toString(), 1500);

      DEBUG_OUT.printf(PSTR(MSG_DBG_CONNECT_SUCCESS), WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
      GVmyLedx.start LED_WIFI_CONNECT_SUCCESS;
      GVmyDisplay.PrintWlanState(WIFI_WLAN);
      GVwifiAPmode = false;
      connected = true;
    } 
    else 
    {
      GVmyDisplay.MessageAdd("\nFailed!",1500);
      DEBUG_OUT.println(F(MSG_DBG_WIFI_NOT_CONNECTED));
      WiFi.disconnect();
    }
    GVwifiMulti.cleanAPlist();
  }
  else DEBUG_OUT.println(F(MSG_DBG_NO_WIFI_CONFIGS_FOUND));

  if (!connected)
  {
    GVwifiAPmode = true;
    GVmyDisplay.PrintMessage("Starting\nAcc. Point\n");
    GVmyDisplay.MessageAdd(GVoilerConf.apn, 1500);
    //WiFi.mode(WIFI_AP);   // only Access Point
    WiFi.softAPConfig(myIP, myIP, IPAddress(255, 255, 255, 0));
    DEBUG_OUT.printf(PSTR(MSG_DBG_START_ACCCESS_POINT), GVoilerConf.apn.c_str(), GVoilerConf.app.c_str());
    
    if (WiFi.softAP(GVoilerConf.apn, GVoilerConf.app))
    {
      delay(500); // Without delay I've seen the IP address blank
      DEBUG_OUT.print(F(MSG_DBG_ACCESS_POINT_IP));
      DEBUG_OUT.println(WiFi.softAPIP());
      GVmyDisplay.PrintWlanState(WIFI_ACCESSPOINT);
      GVmyLedx.add LED_WIFI_CONNECT_FAILED;
      GVmyLedx.add LED_WIFI_CONNECT_SUCCESS;
      GVmyLedx.start();
       /* Setup the DNS server redirecting all the domains to the myIP */
      GVdnsServer.setErrorReplyCode(DNSReplyCode::NoError);
      GVdnsServer.start(DNS_PORT, "*", myIP);
      connected = true;
    }
    else 
    {
      DEBUG_OUT.println(F(MSG_DBG_ACCESS_POINT_FAILED));
      GVmyDisplay.PrintWlanState(WIFI_INACTIVE);
      GVmyLedx.start LED_WIFI_CONNECT_FAILED;
      WiFi.disconnect();
    }
  }
  return connected;
}

/*****************************************************************
 * setup mDNS
 *****************************************************************/
void setupMDNS(void) {
  if (!MDNS.begin(GVoilerConf.apn)) {
    DEBUG_OUT.println(F(MSG_DBG_MDNS_ERROR));
    while(1); // trigger watch dog
  }
  else 
    DEBUG_OUT.printf(PSTR(MSG_DBG_MDNS_STARTED), GVoilerConf.apn.c_str());
}

/*****************************************************************
 * Toggle WiFi
 *****************************************************************/
void toggleWiFi(void)
{
  if (GVwifiSleeping)   // dann aufwecken
  {
    DEBUG_OUT.println(F(MSG_DBG_STARTING_WIFI));
    WiFi.forceSleepWake(); // Wifi on
    delay(100);

    GVmyLedx.on(LED_GRUEN); // LED an waehrend Initialisierung
    bool wifiConnected = setupWiFi();  // dauert bis zu 10s
  
    if (wifiConnected) {
      setupMDNS();
      delay(100);
      GVwifiSleeping = false;
      GVswitchOnMillis = millis();
    }
    GVmyLedx.delay();
  }
  else    // schlafen legen
  {
    DEBUG_OUT.println(F(MSG_DBG_STOPPING_WIFI));
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
 * nur aufgerufen in main loop, wenn WiFi on!
 ***********************************************/
void checkWiFi(void)
{
  if ((GVoilerConf.wts * 1000) < millis() && (GVlastWiFiCheck + 10000) < millis()) {
    if (isMoving() && ((GVswitchOnMillis + 30000) < millis())) 
      toggleWiFi(); // WiFi ggf. ausschalten
    else if (!GVmyLedx.isActive() && !GVmaintenanceMode) 
      GVmyLedx.start LED_SHOW_WIFI_ON;  // nur nicht GVmaintenanceMode, sonst zu viel Geblinke...
    GVlastWiFiCheck = millis();
  }  
}
