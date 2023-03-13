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

#ifndef _configuration_h_
#define _configuration_h_

#include "ValueList.h"

const char PROGMEM * C_OIL_COUNTER_FILE = "/oiler.cnt";
const char PROGMEM * C_CONFIG_FILE      = "/oiler.ini";
const char PROGMEM * C_CONFIG_TEMP_FILE = "/oiler.ini.new";

class Configuration {
  public:
    uint16_t wts = 180;   // Wartezeit Simulation (s)
    uint16_t nmm = 6000;  // Normal Mode Meters
    uint16_t rmm = 3000;  // Regen Mode Meters
    uint16_t omm = 1500;  // Offroad Mode Meters
    uint16_t sim = 20;    // Simulation Meter (Notprogramm)
    uint16_t pac = 3;     // Pump Action Count (Anzahl Pumpaktionen pro Ölvorgang)
    uint16_t glf = 500;   // GPS Low-Filter
    uint16_t otk = 8000;  // Tank Kapazität
    uint16_t use = 0;     // ..davon verbraucht
    String   ffn = UPDATE_FILE;
    uint8_t  fpw = 3;     // Fixpunktabstand beim Schreiben der Tracks in Sekunden (0=no tracks)
    String   apn = "wifiOiler";       // Name eigener Access Point
    String   app = "alteoile";        // Passwort eigener Access Point
    String   uhn = "volw.no-ip.biz";  // Upload Server
    uint16_t uhp = 80;                // Port des Upload Servers
    String   url = "/oilerbase.php";  // php file of oilerbase
    String   bac = "";                // base64 encoded "user:password", empty if no auth. needed
    bool     lgf = true;              // file logging activated
    bool     lgs = true;              // Serial logging activated
    bool     gdl = false;             // GPS Debug logging to file (/gpsLog.txt)
    bool     wso = true;              // WiFi beim Starten automatisch an (true) oder aus (false)?
    String   gts = "20";              // track file filename prefix (if more bikes than oilerbases ;-)

    void read() { this->fworker(true); }
    void write() { this->fworker(false); }
    bool updateOilCounter(){  // must be public
      outFile = _FILESYS.open(C_OIL_COUNTER_FILE, "w");
      if (outFile) {
        outFile.write((const uint8_t *)&this->use, sizeof(this->use));
    //    outFile.write(this->use >> 8);   // simple form
    //    outFile.write(this->use);
        outFile.close();
        return true;
      } 
      return false;      
    }

  private:
    File inFile, outFile;

    //****************** keyWorker() ******************
    // 'value' wird nur beim LESEN der Config genutzt
    // returns 'true', if key has been identified
    bool keyWorker(String key, String value, bool readMode) {
      // in-/outfile should be open
      bool result = true;
      if (key.equals(F("wts"))) {
        if (readMode) this->wts = value.toInt();
        else outFile.print("wts = " + String(this->wts)+'\n');
      }
      else if (key.equals(F("nmm"))) {
        if (readMode) this->nmm = value.toInt();
        else outFile.print("nmm = " + String(this->nmm)+'\n');
      }
      else if (key.equals(F("rmm"))) {
        if (readMode) this->rmm = value.toInt();
        else outFile.print("rmm = " + String(this->rmm)+'\n');
      }
      else if (key.equals(F("omm"))) {
        if (readMode) this->omm = value.toInt();
        else outFile.print("omm = " + String(this->omm)+'\n');
      }
      else if (key.equals(F("sim"))) {
        if (readMode) this->sim = value.toInt();
        else outFile.print("sim = " + String(this->sim)+'\n');
      }
      else if (key.equals(F("pac"))) {
        if (readMode) this->pac = value.toInt();
        else outFile.print("pac = " + String(this->pac)+'\n');
      }
      else if (key.equals(F("glf"))) {
        if (readMode) this->glf = value.toInt();
        else outFile.print("glf = " + String(this->glf)+'\n');
      }
      else if (key.equals(F("otk"))) {
        if (readMode) this->otk = value.toInt();
        else outFile.print("otk = " + String(this->otk)+'\n');
      }
      else if (key.equals(F("ffn"))) {
        if (readMode) this->ffn = String(value);
        else outFile.print("ffn = " + String(this->ffn)+'\n');
      }
      else if (key.equals(F("fpw"))) {
        if (readMode) this->fpw = value.toInt();
        else outFile.print("fpw = " + String(this->fpw)+'\n');
      }
      else if (key.equals(F("apn"))) {
        if (readMode) this->apn = String(value);
        else outFile.print("apn = " + String(this->apn)+'\n');
      }
      else if (key.equals(F("app"))) {
        if (readMode) this->app = String(value);
        else outFile.print("app = " + String(this->app)+'\n');
      }
      else if (key.equals(F("uhn"))) {
        if (readMode) this->uhn = String(value);
        else outFile.print("uhn = " + String(this->uhn)+'\n');
      }
      else if (key.equals(F("uhp"))) {
        if (readMode) this->uhp = value.toInt();
        else outFile.print("uhp = " + String(this->uhp)+'\n');
      }
      else if (key.equals(F("url"))) {
        if (readMode) this->url = String(value);
        else outFile.print("url = " + String(this->url)+'\n');
      }
      else if (key.equals(F("bac"))) {
        if (readMode) this->bac = String(value);
        else outFile.print("bac = " + String(this->bac)+'\n');
      }
      else if (key.equals(F("lgf"))) {
        if (readMode) this->lgf = (value.toInt() == 1);
        else outFile.print("lgf = " + String(this->lgf ? "1" : "0")+'\n');
      }
      else if (key.equals(F("lgs"))) {
        if (readMode) this->lgs = (value.toInt() == 1);
        else outFile.print("lgs = " + String(this->lgs ? "1" : "0")+'\n');
      }
      else if (key.equals(F("gdl"))) {
        if (readMode) this->gdl = (value.toInt() == 1);
        else outFile.print("gdl = " + String(this->gdl ? "1" : "0")+'\n');
      }
      else if (key.equals(F("wso"))) {
        if (readMode) this->wso = (value.toInt() == 1);
        else outFile.print("wso = " + String(this->wso ? "1" : "0")+'\n');
      }
      else if (key.equals(F("gts"))) {
        if (readMode) this->gts = String(value);
        else outFile.print("gts = " + String(this->gts)+'\n');
      }
      else result = false;

      return result;
    }
  	//****************** fworker() ********************
    void fworker(bool readMode) {
      String line;
      uint8_t pos;
      String key, value;
    
      inFile = _FILESYS.open(C_CONFIG_FILE, "r");
      if (!readMode) outFile = _FILESYS.open(C_CONFIG_TEMP_FILE, "w");
      valueList.setAll(false);
      valueList.set("use", true);   // generell raus - wird unten ohne Ausnahme behandelt...
      while(inFile.available()) {
        //Lets read line by line from the file
        line = inFile.readStringUntil('\n');
    
        while (line.charAt(line.length()) == '\r') {
           line = line.substring(0, line.length() - 1);
           line.trim();
        }
           
        if (line.length() > 3 
        && !line.startsWith("//") 
        && (pos = line.indexOf("=")) > 0) {
          key = String(line.substring(0,pos));
          value = String(line.substring(pos+1));
          key.trim(); value.trim();
          valueList.set(key.c_str(), true); // merken, ob ein Schlüssel vorgekommen ist...
          if (!keyWorker(key, value, readMode) && !readMode)
            outFile.print(line+'\n');
        }
        else if (!readMode) outFile.print(line+'\n');
      }
      inFile.close();

      if (readMode) {
        this->readOilCounter();     // Spezialbehandlung für Oil Used Counter
      } else {
        // all params in ini file? let's check:
        char *missedItem;
        while (valueList.getFirst(false, &missedItem)) {
          DEBUG_OUT.printf(PSTR(MSG_DBG_MISSING_CONF_OPTION), missedItem);
          outFile.print("\n");
          keyWorker(String(missedItem), "", readMode);
          valueList.set(missedItem, true);
        }
        outFile.close();
        _FILESYS.remove(C_CONFIG_FILE);
        _FILESYS.rename(C_CONFIG_TEMP_FILE, C_CONFIG_FILE);
        this->updateOilCounter();   // Spezialbehandlung für Oil Used Counter
      }
    }

  	//****************** readOilCounter() ********************
    void readOilCounter(){
      inFile = _FILESYS.open(C_OIL_COUNTER_FILE, "r");
      inFile.read((uint8_t *)&this->use, sizeof(this->use));
      //this->use = inFile.read() * 256 + inFile.read();
      inFile.close();      
    }
} static GVoilerConf;
#endif
