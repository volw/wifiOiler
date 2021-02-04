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
        DEBUG_OUT.print(F(MSG_DBG_ADD_WIFI_TO_MULTIWIFI));
        DEBUG_OUT.println(key.c_str());
        GVwifiMulti.addAP(key.c_str(), value.c_str());
        counter++;
      }
    }
  }
  GVfsUploadFile.close();
  return counter; // Anzahl wifi configs zurückgeben...
}

/*****************************************************************
 * setup WiFi
 * used, but not defined here:
 * -   GVmyIP : IP-Adresse des Access Points
 * - GVoilerConf.apn : SSID von diesem Hotspot
 * - GVoilerConf.app : Passwort, um sich mit diesem Hotspot zu verbinden.
 * Versucht, mit einem bekanntem Wifi Netz zu verbinden - ist keins da oder
 * schlägt die Verbindung fehl, wird ein Access point errichtet...
 */
bool setupWiFi(void) {
  bool connected = false;
  
  DEBUG_OUT.println(F(MSG_DBG_START_SEARCHING_WIFI));
  WiFi.disconnect();  // angeblich werden vorher gemerkte Zugangsdaten damit gelöscht...

  // Daten konfigurierter Netze in LittleFS: /wifi.ini
  if (readWifiData() > 0)
  {
    WiFi.mode(WIFI_STA);   // Only station Mode
    DEBUG_OUT.print(F("[setupWiFi] Connecting Wifi "));
    GVmyDisplay.PrintMessage("Suche WLAN");

    byte count = 0;
    while ((GVwifiMulti.run() != WL_CONNECTED) && (count < 10)) {
      delay(500);
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

      DEBUG_OUT.printf(MSG_DBG_CONNECT_SUCCESS, WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
      //GVmyLedx.start LED_WIFI_CONNECT_SUCCESS;
      GVmyDisplay.PrintWlanState(WIFI_WLAN);
      connected = true;
    } 
    else 
    {
      GVmyDisplay.MessageAdd("\nFailed!",1500);
      DEBUG_OUT.println(F(MSG_DBG_WIFI_NOT_CONNECTED));
      //GVmyLedx.start LED_WIFI_CONNECT_FAILED;
      WiFi.disconnect();
    }
    GVwifiMulti.cleanAPlist();
  }
  else DEBUG_OUT.println(F(MSG_DBG_NO_WIFI_CONFIGS_FOUND));

  if (!connected)
  {
    GVmyDisplay.PrintMessage("Starting\nAcc. Point\n");
    GVmyDisplay.MessageAdd(GVoilerConf.apn, 1500);
    WiFi.mode(WIFI_AP);   // only Access Point
    WiFi.softAPConfig(myIP, myIP, IPAddress(255, 255, 255, 0));
    DEBUG_OUT.printf(MSG_DBG_START_ACCCESS_POINT, GVoilerConf.apn.c_str(), GVoilerConf.app.c_str());
    
    if (WiFi.softAP(GVoilerConf.apn, GVoilerConf.app))
    {
      delay(500); // Without delay I've seen the IP address blank
      DEBUG_OUT.print(F(MSG_DBG_ACCESS_POINT_IP));
      DEBUG_OUT.println(WiFi.softAPIP());
      GVmyDisplay.PrintWlanState(WIFI_ACCESSPOINT);
      connected = true;
    }
    else 
    {
      DEBUG_OUT.println(F(MSG_DBG_ACCESS_POINT_FAILED));
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
    DEBUG_OUT.println(F(MSG_DBG_MDNS_ERROR));
    while(1); // trigger watch dog
  }
  else 
    DEBUG_OUT.printf(MSG_DBG_MDNS_STARTED, GVoilerConf.apn.c_str());
}

/*****************************************************************
 * Toggle WiFi
 *****************************************************************/
void toggleWiFi(void)
{
  if (GVwifiSleeping)
  {
    DEBUG_OUT.println(F(MSG_DBG_STARTING_WIFI));
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
