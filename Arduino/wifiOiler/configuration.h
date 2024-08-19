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

/* To add configuration values, follow these steps:
 *  1. increment #define _LIST_LEN_ (see below)
 *  2. search for "add new option here" (5 locations below)
 *  3. add new value to config.htm (add line to aConfig array)
 */
#pragma once

#define _LIST_LEN_ 23
// #define debug_out(...) Serial.printf(__VA_ARGS__)
// no debug messages:
#define debug_out(...)
const char PROGMEM * C_OIL_COUNTER_FILE = "/oiler.cnt";
const char PROGMEM * C_CONFIG_FILE      = "/oiler.ini";
const char PROGMEM * C_CONFIG_TEMP_FILE = "/oiler.ini.new";

class ValueList {
  
  struct Item {
    char Name[4];
    bool isSet = false;
    bool get() { return isSet; }
    void set(bool _value) { isSet = _value; }
  };

  // add new option here (1/5):
  Item ValueList[_LIST_LEN_] = {
    {"apn"},{"app"},{"bac"},{"ffn"},{"fpw"},{"gdl"},{"glf"},{"gts"},{"lgf"},{"lgs"},
    {"lvl"},{"mrf"},{"nmm"},{"omm"},{"otk"},{"pac"},{"rmm"},{"sim"},{"uhn"},{"uhp"},
    {"url"},{"wso"},{"wts"}
  };

  public:
    void setAll(bool _value) {
      for (uint8_t i = 0; i < _LIST_LEN_; i++) {
        ValueList[i].isSet = _value;
      }
    }
    bool set(const char * name, bool value) {
      uint8_t i = 0;
      while (i < _LIST_LEN_ && strcmp(ValueList[i].Name, name) != 0) i++;
      if (i >= _LIST_LEN_) return false;
      ValueList[i].set(value);
      return true;
    }

    bool getFirst(bool _value, char **foundItem) {
      uint8_t i = 0;
      while (i < _LIST_LEN_ && ValueList[i].get() != _value) i++;
      bool result = (i < _LIST_LEN_);
      if (result) *foundItem = ValueList[i].Name;
      return result;
    }
} static valueList;

class Configuration {
  public:
    String   apn = "wifiOiler";       // Name eigener Access Point
    String   app = "alteoile";        // Passwort eigener Access Point
    String   bac = "";                // base64 encoded "user:password", empty if no auth. needed
    String   ffn = UPDATE_FILE;       // Name der Firmware Datei (wird beim Upload erkannt und Update wird autom. ausgelöst)
    uint8_t  fpw = 3;                 // Fixpunktabstand beim Schreiben der Tracks in Sekunden (0=no tracks)
    bool     gdl = false;             // GPS Debug logging to file (/gpsLog.txt)
    uint16_t glf = 500;               // GPS Low-Filter
    String   gts = "20";              // track file filename prefix (if more bikes than oilerbases ;-)
    bool     lgf = true;              // file logging activated
    bool     lgs = true;              // Serial logging activated
    uint8_t  lvl = _LOG_INFO_;        // Log Level (default = info)
    uint8_t  mrf = 3;                 // max. log files (file rotation)
    uint16_t nmm = 6000;              // Normal Mode Meters
    uint16_t omm = 1500;              // Offroad Mode Meters
    uint16_t otk = 8000;              // Tank Kapazität
    uint16_t pac = 3;                 // Pump Action Count (Anzahl Pumpaktionen pro Ölvorgang)
    uint16_t rmm = 3000;              // Regen Mode Meters
    uint16_t sim = 20;                // Simulation Meter (Notprogramm)
    String   uhn = "volw.no-ip.biz";  // Upload Server
    uint16_t uhp = 80;                // Port des Upload Servers
    String   url = "/oilerbase.php";  // php file of oilerbase
    uint16_t use = 0;                 // ..davon verbraucht (wird nicht in config file gespeichert!!)
    bool     wso = true;              // WiFi beim Starten automatisch an (true) oder aus (false)?
    uint16_t wts = 180;               // Wartezeit Simulation (s)
    /* -------------------------------------------------------*/
    // add new option here (2/5) as member of class "Configuration":
    
    void read() { this->fworker(true); }
    void write() { this->fworker(false); }
    bool updateOilCounter(){  // must be public
      outFile = _FILESYS.open(C_OIL_COUNTER_FILE, "w");
      if (outFile) {
        outFile.write((const uint8_t *)&this->use, sizeof(this->use));
        outFile.close();
        return true;
      } 
      return false;      
    }
    String getJSON()
    {
      String output = "{\"apn\":\"" + String(this->apn) + "\"";
      output += ",\"app\":\"" + String(this->app) + "\"";
      output += ",\"bac\":\"" + String(this->bac) + "\"";
      output += ",\"ffn\":\"" + String(this->ffn) + "\"";
      output += ",\"fpw\":\"" + String(this->fpw) + "\"";
      output += ",\"gdl\":\"" + String(this->gdl) + "\"";
      output += ",\"glf\":\"" + String(this->glf) + "\"";
      output += ",\"gts\":\"" + String(this->gts) + "\"";
      output += ",\"lgf\":\"" + String(this->lgf) + "\"";
      output += ",\"lgs\":\"" + String(this->lgs) + "\"";
      output += ",\"lvl\":\"" + String(this->lvl) + "\"";
      output += ",\"mrf\":\"" + String(this->mrf) + "\"";
      output += ",\"nmm\":\"" + String(this->nmm) + "\"";
      output += ",\"omm\":\"" + String(this->omm) + "\"";
      output += ",\"otk\":\"" + String(this->otk) + "\"";
      output += ",\"pac\":\"" + String(this->pac) + "\"";
      output += ",\"rmm\":\"" + String(this->rmm) + "\"";
      output += ",\"sim\":\"" + String(this->sim) + "\"";
      output += ",\"uhn\":\"" + String(this->uhn) + "\"";
      output += ",\"uhp\":\"" + String(this->uhp) + "\"";
      output += ",\"url\":\"" + String(this->url) + "\"";
      output += ",\"use\":\"" + String(this->use) + "\"";
      output += ",\"wso\":\"" + String(this->wso) + "\"";
      output += ",\"wts\":\"" + String(this->wts) + "\"";
      // add new option here (3/5) for JSON output
      output += "}";
      return output;      
    }

    bool set(String key, String value)
    {
      //+++ Parameter prüfen, ob bei Änderung Aktion ausgelöst werden muss/sollte
           if (key.equals(F("apn"))) this->apn = value;         // Name eigener Access Point
      else if (key.equals(F("app"))) this->app = value;         // Passwort eigener Access Point
      else if (key.equals(F("bac"))) this->bac = value;         // base64 encoded "user:password", empty if no auth. needed
      else if (key.equals(F("ffn"))) this->ffn = value;         // Name der Firmware Datei (wird beim Upload erkannt und Update wird autom. ausgelöst)
      else if (key.equals(F("fpw"))) this->fpw = value.toInt(); // Fixpunktabstand beim Schreiben der Tracks in Sekunden (0=no tracks)
      else if (key.equals(F("gdl"))) this->gdl = toBool(value); // GPS Debug logging to file (/gpsLog.txt)
      else if (key.equals(F("glf"))) this->glf = value.toInt(); // GPS Low-Filter
      else if (key.equals(F("gts"))) this->gts = value;         // track file filename prefix (if more bikes than oilerbases ;-)
      else if (key.equals(F("lgf"))) this->lgf = toBool(value); // file logging activated
      else if (key.equals(F("lgs"))) this->lgs = toBool(value); // Serial logging activated
      else if (key.equals(F("lvl"))) { 
        this->lvl = value.toInt();                              // Log Level (default = info)
        StaticLogger.setLogLevel(this->lvl);
      }
      else if (key.equals(F("mrf"))) this->mrf = value.toInt(); // max. log files (file rotation)
      else if (key.equals(F("nmm"))) this->nmm = value.toInt(); // Normal Mode Meters
      else if (key.equals(F("omm"))) this->omm = value.toInt(); // Offroad Mode Meters
      else if (key.equals(F("otk"))) this->otk = value.toInt(); // Tank Kapazität
      else if (key.equals(F("pac"))) this->pac = value.toInt(); // Pump Action Count (Anzahl Pumpaktionen pro Ölvorgang)
      else if (key.equals(F("rmm"))) this->rmm = value.toInt(); // Regen Mode Meters
      else if (key.equals(F("sim"))) this->sim = value.toInt(); // Simulation Meter (Notprogramm)
      else if (key.equals(F("uhn"))) this->uhn = value;         // Upload Server
      else if (key.equals(F("uhp"))) this->uhp = value.toInt(); // Port des Upload Servers
      else if (key.equals(F("url"))) this->url = value;         // php file of oilerbase
      else if (key.equals(F("use"))) this->use = value.toInt(); // ..davon verbraucht
      else if (key.equals(F("wso"))) this->wso = toBool(value); // WiFi beim Starten automatisch an (true) oder aus (false)?
      else if (key.equals(F("wts"))) this->wts = value.toInt(); // Wartezeit Simulation (s)
      // add new option here (4/5) to if else if structure
      else return false;
      return true;
    }

  private:
    File inFile, outFile;

    bool toBool(String str) {
      str.toUpperCase();
      return (str.startsWith("TRUE") || str.startsWith("1") || str.startsWith("JA") || str.startsWith("WAHR"));
    }

    //****************** keyWriter() ******************
    // returns 'true', if key has been identified
    // outfile should be open
    //*************************************************
    bool writeKey(String key, String value) 
    {
           if (key.equals(F("apn"))) outFile.printf("apn = %s\n", this->apn.c_str());         // Name eigener Access Point                                                          
      else if (key.equals(F("app"))) outFile.printf("app = %s\n", this->app.c_str());         // Passwort eigener Access Point                                                      
      else if (key.equals(F("bac"))) outFile.printf("bac = %s\n", this->bac.c_str());         // base64 encoded "user:password", empty if no auth. needed                           
      else if (key.equals(F("ffn"))) outFile.printf("ffn = %s\n", this->ffn.c_str());         // Name der Firmware Datei (wird beim Upload erkannt und Update wird autom. ausgelöst)
      else if (key.equals(F("fpw"))) outFile.printf("fpw = %d\n", this->fpw);                 // Fixpunktabstand beim Schreiben der Tracks in Sekunden (0=no tracks)                
      else if (key.equals(F("gdl"))) outFile.printf("gdl = %s\n", this->gdl ? "ja" : "nein"); // GPS Debug logging to file (/gpsLog.txt)                                            
      else if (key.equals(F("glf"))) outFile.printf("glf = %d\n", this->glf);                 // GPS Low-Filter                                                                     
      else if (key.equals(F("gts"))) outFile.printf("gts = %s\n", this->gts.c_str());         // track file filename prefix (if more bikes than oilerbases ;-)                      
      else if (key.equals(F("lgf"))) outFile.printf("lgf = %s\n", this->lgf ? "ja" : "nein"); // file logging activated                                                             
      else if (key.equals(F("lgs"))) outFile.printf("lgs = %s\n", this->lgs ? "ja" : "nein"); // Serial logging activated                                                           
      else if (key.equals(F("lvl"))) outFile.printf("lvl = %d\n", this->lvl);                 // Log Level (default = info)                                                         
      else if (key.equals(F("mrf"))) outFile.printf("mrf = %d\n", this->mrf);                 // max. log files (file rotation)
      else if (key.equals(F("nmm"))) outFile.printf("nmm = %d\n", this->nmm);                 // Normal Mode Meters                                                                 
      else if (key.equals(F("omm"))) outFile.printf("omm = %d\n", this->omm);                 // Offroad Mode Meters                                                                
      else if (key.equals(F("otk"))) outFile.printf("otk = %d\n", this->otk);                 // Tank Kapazität                                                                     
      else if (key.equals(F("pac"))) outFile.printf("pac = %d\n", this->pac);                 // Pump Action Count (Anzahl Pumpaktionen pro Ölvorgang)                              
      else if (key.equals(F("rmm"))) outFile.printf("rmm = %d\n", this->rmm);                 // Regen Mode Meters                                                                  
      else if (key.equals(F("sim"))) outFile.printf("sim = %d\n", this->sim);                 // Simulation Meter (Notprogramm)                                                     
      else if (key.equals(F("uhn"))) outFile.printf("uhn = %s\n", this->uhn.c_str());         // Upload Server                                                                      
      else if (key.equals(F("uhp"))) outFile.printf("uhp = %d\n", this->uhp);                 // Port des Upload Servers                                                            
      else if (key.equals(F("url"))) outFile.printf("url = %s\n", this->url.c_str());         // php file of oilerbase                                                              
      else if (key.equals(F("wso"))) outFile.printf("wso = %s\n", this->wso ? "ja" : "nein"); // WiFi beim Starten automatisch an (true) oder aus (false)?                          
      else if (key.equals(F("wts"))) outFile.printf("wts = %d\n", this->wts);                 // Wartezeit Simulation (s)   
      // add new option here (5/5) to if else if structure
      else return false;
      return true;
    }
    
    //****************** fworker() ********************
    void fworker(bool readMode) 
    {
      String line;
      uint8_t pos;
      String key, value;
    
      inFile = _FILESYS.open(C_CONFIG_FILE, "r");
      if (!readMode) outFile = _FILESYS.open(C_CONFIG_TEMP_FILE, "w");
      valueList.setAll(false);
      while(inFile.available()) {
        //Lets read line by line from the file
        line = inFile.readStringUntil('\n');
        debug_out("Zeile: %s\n", line.c_str());
    
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
          debug_out("Key = '%s', Value = '%s'\n", key.c_str(), value.c_str());
          valueList.set(key.c_str(), true); // merken, ob ein Schlüssel vorgekommen ist...
          if (readMode) 
          {
            if (this->set(key, value)) debug_out("key was set\n");
            else debug_out("key was NOT set");
          }
          else if (!writeKey(key, value))
          {
            outFile.printf("%s\n", line.c_str());
          }
        }
        else if (!readMode) 
        {
          outFile.printf("%s\n", line.c_str());
        }
      }
      inFile.close();
      debug_out("closed inFile\n");

      if (!readMode) {
        debug_out("checking valueList\n");
        // all params in ini file? let's check:
        char *missedItem;
        while (valueList.getFirst(false, &missedItem)) 
        {
          debug_out(PSTR("[%s] Option nicht in ini Datei: %s\n"),__FUNCTION__,missedItem);
          outFile.printf("\n");
          writeKey(String(missedItem), "");
          valueList.set(missedItem, true);
        }
        outFile.close();
        _FILESYS.remove(C_CONFIG_FILE);
        _FILESYS.rename(C_CONFIG_TEMP_FILE, C_CONFIG_FILE);

        this->updateOilCounter();   // Spezialbehandlung für Oil Used Counter
      } else {
        this->readOilCounter();     // Spezialbehandlung für Oil Used Counter
      }
      debug_out("Leaving fWorker\n");
    }
    //****************** readOilCounter() ********************
    void readOilCounter(){
      inFile = _FILESYS.open(C_OIL_COUNTER_FILE, "r");
      inFile.read((uint8_t *)&this->use, sizeof(this->use));
      //this->use = inFile.read() * 256 + inFile.read();
      inFile.close();      
    }    
} static GVoilerConf;
