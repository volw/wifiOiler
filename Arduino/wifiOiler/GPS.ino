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

/******************************************
 * setupGPS()
 ******************************************/
void setupGPS(void)
{
  // GPS initialisieren:
  if (!GVmaintenanceMode) {
    debugPrintf(PSTR(MSG_DBG_SETUP_GPS_INIT));
    gpsSerial.begin(9600);  // einige GPS Module senden nur mit 4800 baud...
    delay(100);
  }
  
  // fuer Notlauffunktion vorbereiten
  GVlastGPSData = millis();
}

/************************************************
 * Jedes gelesene Zeichen vom GPS Modul wird hier
 * durchgeschleust.
 * Rueckgabe true, wenn Satzende erreicht wurde.
 ************************************************/
bool evalGPS(char data) {
  GVgpsBuffer[GVgpsBufferIndex] = data;
  GVgpsBuffer[GVgpsBufferIndex + 1] = '\0';
  // wenn Puffer voll, werden restliche Zeichen verschluckt...
  if (GVgpsBufferIndex < GPS_BUFFER_LENGTH - 1) GVgpsBufferIndex++;
  return (data == '\n');
}

/************************************************************************************
 * Lesen von GPS Daten und ggf. Auswertung auslösen
 * Im Wartungsmodus keine Auswertung, um MCU damit nicht zu belasten
 ************************************************************************************/
void checkGPSdata() {
  if (!GVmaintenanceMode)
  {
    while (gpsSerial.available())
    {
      if (evalGPS(gpsSerial.read())) // true, if eol
      {
        if (GVoilerConf.gdl) {
          GVoutFile = _FILESYS.open(GPSLogFile, "a");
          GVoutFile.print(GVgpsBuffer);
          GVoutFile.close();
        }

        // nur RMC und GGA Saetze auswerten:
        bool isGGA = isGGArecord();
        if (isGGA || isRMCrecord())
        {
          GVmyDisplay.PrintGpsState(GPS_ACTIVE);
          GVlastGPSData = millis();
                    
          // TinyGPSPlus Objekt fuettern:
          for (int i = 0; GVgpsBuffer[i]; i++) GVgpsNew.encode(GVgpsBuffer[i]);
  
          if (isGGA) analyzeGPSInfo();
        }
        GVgpsBufferIndex = 0;
      }
    }
  }
  
  if ((GVlastGPScheck + 1000) < millis()) // not more than once per second
  {
    if ((GVlastGPSData + 3000) < millis())
    {
      GVmyDisplay.PrintGpsState(GPS_NONE);  // update display when not GGA-/RMC-Records for more than 3s
    }

    // Wenn die letzte Koordinate zu alt, Karenzzeit abgelaufen und KEIN Wartungsmodus, dann simulieren:
    if ((GVoldLocationAge + 3000) < millis() && (GVoilerConf.wts * 1000) < millis() && !GVmaintenanceMode)
    {
      // nach der Karenzzeit und 3s keine GPS-Daten = simulieren
      GVmeterSincePump += GVoilerConf.sim;
    }
    GVlastGPScheck = millis();
  
    GVmyDisplay.PrintMeter(getModeMeters(GVpumpMode) - GVmeterSincePump);  
    
    // egal, wie oben die Berechnung war, hier die Entscheidung, ob gepumpt werden muss..
    // aber nicht oefter als 1 mal pro Sekunde. Ausserdem funktioniert Dauerpumpen so auch im WartungsMode:
    if (GVpumpMode != MODE_OFF && GVmeterSincePump >= getModeMeters(GVpumpMode))
    {
      TriggerPump();
    }
  }
  else if (GVpumpMode == MODE_PERMANENT) TriggerPump(); // nicht warten - active-check in TriggerPump()
}

/*************************************************************
 * Check if GPS record is of type GGA, no matter which device
 * Buffer must be filled. Simple check for optimized speed
 ************************************************************/
bool isGGArecord()
{
  // Abfrage auf "$..GGA" (wobei . beliebiges Zeichen)
  // auf die Übergabe des Puffers als Parameter wird aus Optimierungsgründen verzichtet
  if (GVgpsBuffer[0] != '$') return false;
  if (GVgpsBuffer[1] == 0 || GVgpsBuffer[2] == 0) return false;
  if (GVgpsBuffer[3] != 'G' || GVgpsBuffer[4] != 'G' || GVgpsBuffer[5] != 'A') return false;
  return true;
}

/*************************************************************
 * Check if GPS record is of type RMC, no matter which device
 * Buffer must be filled. Simple check for optimized speed
 ************************************************************/
 bool isRMCrecord()
{
  // Abfrage auf "$..RMC" (wobei . beliebiges Zeichen)
  // auf die Übergabe des Puffers als Parameter wird aus Optimierungsgründen verzichtet
  if (GVgpsBuffer[0] != '$') return false;
  if (GVgpsBuffer[1] == 0 || GVgpsBuffer[2] == 0) return false;
  if (GVgpsBuffer[3] != 'R' || GVgpsBuffer[4] != 'M' || GVgpsBuffer[5] != 'C') return false;
  return true;
}


/********************************************************
 * Distanzberechnung anhand von GPS Daten (wenn moeglich)
 * Wird aufgerufen, wenn ein vollstaendiger Satz an GPS 
 * Saetzen in das TinyGPS++ Objekt gefuettert wurde..
 * Gibt wahr zurueck, wenn der Satz Daten aktuell war.
 ********************************************************/
bool analyzeGPSInfo() {
  float distance = 0;
  bool filter = false;
  bool isUpdated = GVgpsNew.location.isUpdated();   // benoetige ich unten noch mal

  GVmyDisplay.PrintFixState(isUpdated);
  if (GVgpsNew.speed.isUpdated()) GVmyDisplay.PrintKmh(GVgpsNew.speed.kmph());

  if (isUpdated)  // wahr, wenn aktuelle GPS-Daten vorliegen
  {
    if (GVoldLat != 0 || GVoldLng != 0)  // alte Koordinate vorhanden?
    {
      distance = GVgpsNew.distanceBetween(GVoldLat, GVoldLng, GVgpsNew.location.lat(), GVgpsNew.location.lng());
      // Low-Pass Filter:
      if ((distance * 100) <= GVoilerConf.glf)  // LowPass Filter (in cm)
      {
        distance = 0;   // Simulation: auf der Stelle bleiben
        filter = true;
      } 
      else if (millis() > (GVoldLocationAge + MAX_OLD_LOCATION_AGE)) // wenn die alte Koordinate zu alt ist..
      {
        // ..wird doch lieber simuliert (ausserhalb Karenzzeit):
        distance = simMeters();
      }
      else  // hier hat eine "echte" Bewegung stattgefunden:
      {
        GVtotalDist += distance;
      }
    }
    else  // simulieren:
    {
      distance = simMeters();
    }
    if (!filter)  // sonst wird praktisch Verbleib auf alter Koordinate simuliert
    {
      GVoldLat = GVgpsNew.location.lat();
      GVoldLng = GVgpsNew.location.lng();  // nach Abruf ist isUpdated() unbrauchbar
    }
    GVoldLocationAge = millis();
  }
// hier keine Simulation nötig, wird an aufrufender Stelle gemacht...
  GVmeterSincePump += distance;
  
  if (isUpdated && !filter) // nur dann Satz in Datei schreiben
  {
    writeGPSInfo(distance);
  }
  return isUpdated;
}

/**************************************************************
 * Create track file name with date and time from GPS
 * File name is formed according to this pattern: YYMMDD-HHMM.dat
 * Track files will be identified by this pattern! (20*.dat)
 **************************************************************/
bool createDateFilename(uint32_t date, uint32_t time) {
  if (date == 0) return false;
  // time könnte tatsächlich 0 sein...
  
  char buffer[] = "YYMMDD";

  for (uint8_t i = 0; i < 3; i++)
  {
    buffer[2*i+1] = (date % 10) + '0';
    date /= 10;
    buffer[2*i] = (date % 10) + '0';
    date /= 10;
  }
  buffer[8] = '\0';

  time /= 10000;  // Sekunden und Millisekunden raus...
  GVgpsTrackFilename = GVoilerConf.gts + String(buffer) + "-" + ((time < 1000) ? "0" + String(time) : String(time));
  debugPrintf(PSTR(MSG_DBG_GPS_DATE_TIME), GVgpsTrackFilename);
  GVgpsTrackFilename = "/" + GVgpsTrackFilename + ".dat";
 
  return true;
}


/***********************************************
 * Check Movement
 * --------------
 * Jede Sekunde wird geprüft, ob sich die total gefahrene (nur per GPS gemessene) Strecke geändert hat.
 * Wenn das so ist, wird ein Zähler erhöht (bis max. MAX_MOVEMENT_COUNT, derzeit 30).
 * Ist dieser Zählerstand also 30, bedeutet dies, dass sich die Strecke 
 * in den letzten 30 Sekunden sekündlich geändert hat -> die Karre also rollt!!
 ***********************************************/ 
void checkMovement()
{
  if (((GVoilerConf.wts * 1000) < millis()) && ((GVlastMovementCheck + 1000) < millis()))
  {
    if (GVlastTotalDist != GVtotalDist) // es hat eine Bewegung stattgefunden
    {
      if (GVmovementCounter < MAX_MOVEMENT_COUNT) GVmovementCounter++;
      GVlastTotalDist = GVtotalDist;
    }
    else  // keine Bewegung
    {
      if (GVmovementCounter > 0) GVmovementCounter--;
    }
    GVlastMovementCheck = millis();
    
    // Hier noch eine Ausgabe für's Log:
    if (isMoving() && !GVlastMovingState)
    {
      GVlastMovingState = true;
      debugPrintf(PSTR(MSG_DBG_MOVEMENT_MOTO_MOVING));
    }
    if (isHalting() && GVlastMovingState)
    {
      GVlastMovingState = false;
      debugPrintf(PSTR(MSG_DBG_MOVEMENT_MOTO_STOPPED));
    }    
    GVmyDisplay.PrintMoveState(GVlastMovingState);
  }
}


/********************************************************
 * Ausgabe aufbereiteter Daten in Datei (eigenes Format)
 * Wird aufgerufen, wenn ein vollstaendiger Satz an GPS 
 * Daten in das TinyGPS++ Objekt gefuettert wurde..
 * Format: s. struct
 ********************************************************/
void writeGPSInfo(float dist) {
  if (GVcurrentfpw == 0) 
  {
    if (!GVmyLedx.isActive()) GVmyLedx.start LED_GPS_WRITE_NO_TRACKS;
    GVmyDisplay.PrintFileState(false);
    return;  // bei 0 keine Tracks...
  }
  
  if (!GVvalidGPSFilename)  // es gibt noch keinen Dateinamen - aber so gut wie sicher Datum und Uhrzeit...
  {
    // return == false, falls Datum noch 0 war...
    GVvalidGPSFilename = createDateFilename(GVgpsNew.date.value(), GVgpsNew.time.value());
  }

  // wenn nur alle paar Sekunden geschrieben wird, sollte der dist Wert aufkumuliert werden 
  GVcumDist += dist;

  if (!GVvalidGPSFilename)
  {
    GVmyDisplay.PrintFileState(false);
    return;
  }
  if ((GVlastGPSwrite + (long)(GVoilerConf.fpw * 1000L) - 300L) > millis())  // -300, um Sekundengrenze nicht zu treffen (unwahrscheinlich - aber möglich)
    return; 

  GVoutFile = _FILESYS.open(GVgpsTrackFilename, "a");
  if (GVoutFile) {
    GVgps.data.datum  = GVgpsNew.date.isValid() ? GVgpsNew.date.value() : 0;
    GVgps.data.zeit   = GVgpsNew.time.isValid() ? GVgpsNew.time.value() : 0;
    if (GVgpsNew.location.isValid()) {
      GVgps.data.loclat = GVgpsNew.location.lat();
      GVgps.data.loclng = GVgpsNew.location.lng();
    } else {
      GVgps.data.loclat = 0;
      GVgps.data.loclng = 0;
    }
    GVgps.data.hdop   = (float)(GVgpsNew.hdop.isValid() ? GVgpsNew.hdop.value() : 0);
    GVgps.data.speed  = (float)(GVgpsNew.speed.isValid() ? GVgpsNew.speed.kmph() : 0);
    GVgps.data.alt    = (float)(GVgpsNew.altitude.isValid() ? GVgpsNew.altitude.meters() : 0);
    GVgps.data.locage = GVgpsNew.location.age();
    GVgps.data.oldage = (millis()-GVoldLocationAge);
    GVgps.data.dist   = GVcumDist;
    GVgps.data.msp    = GVmeterSincePump;
    GVgps.data.pmode  = getModeMeters(GVpumpMode);
  
    GVoutFile.write((uint8_t*)GVgps.buffer, sizeof(GVgps.buffer));
    GVoutFile.close();
    GVlastGPSwrite = millis();
    GVcumDist = 0;

    // alle MIN_MINUTES_FREE-1 nachsehen, ob noch Platz im Filesystem ist - sonst Aufzeichnung einstellen
    if (((millis() - GVlastFScheck) / 60000) > MIN_MINUTES_FREE-1) 
    {
      checkFilesystemSpace();
    }
    if (!GVmyLedx.isActive()) GVmyLedx.start LED_GPS_WRITE_SUCCESS;
    GVtotalGPSwrite++;
    GVmyDisplay.PrintFileState(true);
  }
  else // error writing to file
  {
    if (!GVmyLedx.isActive()) GVmyLedx.start LED_GPS_WRITE_FAILED;
    GVtotalWriteErrors++;
    GVmyDisplay.PrintFileState(false);
  }
}
