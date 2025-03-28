/************************************************************************
 * wifiOiler, an automatic distance depending motorbike chain oiler
 * Copyright (C) 2019-2024, volw
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
 // ACHTUNG: defines in wifioiler.h beachten!!
#include <ESP8266WiFiMulti.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <TinyGPS++.h>
#include "src/biLedx/biLedx.h"  // lib für nicht-modales Blinken (zweifarbiger) LEDs
#include "wifiOiler.h"
#include "LittleFSlogger.h"
#include "oilerDisplay.h"
#include "messages_de.h"
//#include "messages_en.h"
#include "globalVars.h"
#include "configuration.h"
#include "rotate.h"

/*******************************************************************
 ******************** Setup ****************************************
 *******************************************************************/
void setup() 
{    
  GVmyLedx.on(LED_GRUEN); // LED an waehrend Initialisierung
  pinMode(BUTTON_PIN, OUTPUT);
  digitalWrite(BUTTON_PIN, LOW);
    
  Serial.begin(9600);
  delay(100); // = less garbage...
  Serial.println();

  GVmyDisplay.Init();
  GVmyDisplay.PrintMessage("Init", 10000);

  bool deleteLog = false;
  if (!_FILESYS.begin()) {
    Serial.println(F(MSG_ERROR_INITIALISING_FILESYSTEM));
  } else {  // read config / rotate log / check log file size...
    GVoilerConf.read();
    if (_FILESYS.exists(LOG_FILE_NAME)) {
      // try to rotate:
      if (GVoilerConf.mrf > 0) {
        rotateFile(LOG_FILE_NAME, GVoilerConf.mrf);
      }
      // still there and too big?
      if (_FILESYS.exists(LOG_FILE_NAME)) {
        File f = _FILESYS.open(LOG_FILE_NAME, "r");
        deleteLog = (f.size() > LOG_FILE_MAX_BYTES);
        f.close();
      }
    }
  }

  // schon mal was auf's Display bringen (wird erst in Loop angezeigt über Check()):
  GVmyDisplay.PrintModeStr(getPumpModeStr(GVpumpMode));
  GVmyDisplay.PrintMeter(getModeMeters(GVpumpMode));
  GVmyDisplay.PrintKmh(0);

  GVmyDisplay.MessageAdd(".", 10000);
  GVcurrentfpw = GVoilerConf.fpw;  // merken, GVcurrentfpw bestimmt je nach Platz im FS, ob Tracks geschrieben werden...

  // Logging in Datei und Serial (sonst als 2. Parameter: false)
  // VORSICHT: falls Datei sehr groß wird, können Schreiboperationen (incl.
  // Öffnen und schließen der Datei) seeehr langsam sein und das ganze Ding ausbremsen.
  if (!StaticLogger.begin(GVoilerConf.lgf ? LOG_FILE_NAME : "", GVoilerConf.lgs)) {
    Serial.println(F(MSG_ERROR_INITIALISING_LOGGER));
  }
  StaticLogger.setLogLevel(GVoilerConf.lvl);
  GVmyDisplay.MessageAdd(".", 10000);
  
  infoPrintf(PSTR(MSG_PROGRESS_STARTING_INIT));
  if (deleteLog) warnPrintf(PSTR(MSG_CHECK_LOG_SIZE_DELETE));

  setupButton();

  //TODO: +++hier könnte noch ein Check auf Track-Dateien < x bytes erfolgen - die dann löschen

  checkforUpdate();
  GVmyDisplay.MessageAdd(".", 10000);

  checkFilesystemSpace(); // tracking deaktivieren, wenn zu wenig Platz
  GVmyDisplay.MessageAdd(".", 10000);

  // WiFi wird beim Starten ausgeschaltet und kann bei Bedarf (Button) angeschaltet werden:
  WiFi.forceSleepBegin(); // Wifi off
  GVwifiSleeping = true;

  setupGPS(); // Abfrage auf Wartungsmodus innerhalb der Funktion
  GVmyDisplay.MessageAdd(".", 10000);

  setupWebServer();
  GVhttp.setUserAgent(F(HTTP_USER_AGENT));  // only allow this user agent on oilerbase (see php scripts)
  GVmyDisplay.MessageAdd(".", 10000);

  checkTank();  // Öltank leer?

  infoPrintf(PSTR(MSG_VERSION_MESSAGE), VERSION, BOARD_TYPE);

  if (GVoilerConf.wso || GVmaintenanceMode) 
    toggleWiFi();
  else 
    GVmyLedx.off();
  GVmyDisplay.PrintMessage("wifiOiler\n  v");
  GVmyDisplay.MessageAdd(VERSION,2000);
  #ifdef _NO_PUMP_
    warnPrintf(PSTR(MSG_ATT_PUMP_DEACTIVATED));
    GVmyDisplay.PrintAckMessage("Pumpe\ndeaktiv.\n(confirm)");
  #endif
  infoPrintf(PSTR(MSG_PROGRESS_STARTING_END));
}

/*******************************************************************
 ***************** Main Loop ***************************************
 *******************************************************************/
void loop() {
  if (!GVwifiSleeping) {
    if (GVwifiAPmode) {
      GVdnsServer.processNextRequest();
    } 
    MDNS.update();
    GVwebServer.handleClient();
    checkWiFi();
  }
  GVmyLedx.checkState();  // LED-Behandlung
  GVmyPumpx.checkState(); // Behandlung Pumpe
  checkButton();          // Button Abfrage  
  checkGPSdata();         // GPS Daten lesen, falls welche da sind
  checkMovement();        // Überprüfung auf Bewegung
  GVmyDisplay.Check();    // Display handling
  
  if (GVmaintenanceMode && (millis() > (GVshowMaintMode + 10000UL))) {
    // noch ne kleine Spezialroutine, um Wartungsmodus anzuzeigen:
    GVmyLedx.start LED_SHOW_MAINTENANCE;
    GVshowMaintMode = millis();
  }
  if (GVupdateResult == UPD_REBOOT_NOW) {
    checkforUpdate();
    GVupdateResult = UPD_NOT_ACTIVE; // falls er wieder zurückkommt...
  }
}

/*******************************************************************
 ******************** Reboot ***************************************
 *******************************************************************/
void oilerReboot() {
  if (!GVwifiSleeping) {
    //MDNS.close();
    WiFi.disconnect();
    delay(500);
  }
  ESP.restart();
}
