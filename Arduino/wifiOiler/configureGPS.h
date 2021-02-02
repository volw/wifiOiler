/****
 *      wifiOiler, an automatic distance depending motorbike chain oiler
 *      Copyright (C) 2019-2021, volw
 *
 *      This program is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program.  If not, see <https://www.gnu.org/licenses/>.
 ****/

/**
 * STILL UNDER CONSTRUCTION !!!
 */ 


// Send a byte array of UBX protocol to the GPS
// Quelle: https://ukhas.org.uk/guides:falcom_fsa03#sample_code
void sendUBX(uint8_t *MSG, uint8_t len) {
  for(int i=0; i<len; i++) {
    Serial.print(MSG[i]);
    // ursprünglich hieß es hier:
    //Serial.print(MSG[i], BYTE);
    // das "BYTE" Schlüsselwort wird aber nicht mehr unterstützt
  }
  //Serial.println();
}
 
 
// Calculate expected UBX ACK packet and parse UBX response from GPS
// Quelle: https://ukhas.org.uk/guides:falcom_fsa03#sample_code
boolean getUBX_ACK(uint8_t *MSG) {
  uint8_t b;
  uint8_t ackByteID = 0;
  uint8_t ackPacket[10];
  unsigned long startTime = millis();
  DEBUG_OUT.print(" * Reading ACK response: ");
 
  // Construct the expected ACK packet    
  ackPacket[0] = 0xB5;  // header
  ackPacket[1] = 0x62;  // header
  ackPacket[2] = 0x05;  // class
  ackPacket[3] = 0x01;  // id
  ackPacket[4] = 0x02;  // length
  ackPacket[5] = 0x00;
  ackPacket[6] = MSG[2];  // ACK class
  ackPacket[7] = MSG[3];  // ACK id
  ackPacket[8] = 0;   // CK_A
  ackPacket[9] = 0;   // CK_B
 
  // Calculate the checksums
  for (uint8_t i=2; i<8; i++) {
    ackPacket[8] = ackPacket[8] + ackPacket[i];
    ackPacket[9] = ackPacket[9] + ackPacket[8];
  }
 
  while (1) {
 
    // Test for success
    if (ackByteID > 9) {
        // All packets in order!
        DEBUG_OUT.println(" (SUCCESS!)");
        return true;
    }
 
    // Timeout if no valid response in 3 seconds
    if (millis() - startTime > 3000) { 
      DEBUG_OUT.println(" (FAILED!)");
      return false;
    }
 
    // Make sure data is available to read
    if (Serial.available()) {
      b = Serial.read();
 
      // Check that bytes arrive in sequence as per expected ACK packet
      if (b == ackPacket[ackByteID]) { 
        ackByteID++;
        //Serial.print(b, HEX);
      } else {
        ackByteID = 0;  // Reset and look again, invalid order
      }
    }
  }
}

/*************************************
 * HEX Wert in dezimal umrechnen
 *************************************/
uint8_t getDecValue(char c)
{ 
  // '0' = 48
  // '9' = 57
  // 'A' = 65 ('a' =  97)
  // 'F' = 70 ('f' = 102)
  if (c > 'F') c -= 32;
  if (c >= 65) return (c-55);
  return (c-48);
}

/**********************************************
 * Apply GPS Config Commands from file
 **********************************************/ 
void configureGPS(void)
{
  //TODO noch in Entwicklung...
  DEBUG_OUT.println(F("[configureGPS] still under construction..."));
  
  char gpsConfig[MAX_GPS_CONFIG_COMMAND] = "\0";
  uint8_t i = 0;  // gpsConfig Index
  uint8_t l = 0;  // line index (String)
  uint8_t len = 0;// length of string
  char c;
  String line;

  //TODO debug entfernen:
  DEBUG_OUT.println("reading gps device config file " + GVoilerConf.gcf);
  
  GVfsUploadFile = _FILESYS.open(GVoilerConf.gcf.startsWith("/") ? GVoilerConf.gcf : "/" + GVoilerConf.gcf, "r");
  while(GVfsUploadFile.available()) {
    //Lets read line by line from the file
    line = GVfsUploadFile.readStringUntil('\n');
    line.trim();

    //TODO debug entfernen:
    //DEBUG_OUT.println(line);
    //DEBUG_OUT.print(F("Länge der Zeile:"));
    len = line.length();
    //DEBUG_OUT.println(len);
    
    // nicht schön - aber funktioniert...
    l = 0;
    if ((len > 0) && !line.startsWith("#"))   // comment line
    {
      if (line.startsWith("\""))  // String command (Zeile mit umklammernden "")
      {
        line = line.substring(1,len-1);   // strip ""
        i = len - 2;                      // adjust to new length
      }
      else 
      {
        while (l < len) 
        {
          c = line.charAt(l);
          if (c == 32) 
          {
            i++;
            gpsConfig[i] = 0;
          }
          else 
          {
            gpsConfig[i] = (gpsConfig[i] * 16) + getDecValue(c);
          }
          l++;
        }
        i++;
        gpsConfig[i] = 0;
        line = String(gpsConfig);
      }
    }
    
    if (i > 0) // könnte auch auf Mindestanzahl bytes für ein Kommando abfragen...
    {
      //TODO debug output entfernen:
      if (line.charAt(0) == '$')
        DEBUG_OUT.print(line);
      else
        for (int j = 0; j < i; j++)
        {
          DEBUG_OUT.print(line.charAt(j), DEC);
          DEBUG_OUT.print(" ");
        }
      DEBUG_OUT.println();
      
      //+++ send command and reset buffer
      //TODO - hier senden -
      // array muss gesendet werden, da auch 0-Werte enthalten sein können.
      gpsConfig[0] = i = 0;
    }
  }
  GVfsUploadFile.close();
}
