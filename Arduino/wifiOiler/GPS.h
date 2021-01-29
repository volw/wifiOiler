// GVgpsNew.date.value()..........: uint32_t   4
// GVgpsNew.time.value()..........: uint32_t   4
// GVgpsNew.location.lat()........: double     8
// GVgpsNew.location.lng()........: double     8
// GVgpsNew.hdop.value()..........: float      4
// GVgpsNew.speed.kmph()..........: float      4
// GVgpsNew.altitude.meters().....: float      4
// GVgpsNew.location.age()........: uint32_t   4
// millis()-GVoldLocationAge......: uint32_t   4
// dist...........................: float      4
// GVmeterSincePump...............: float      4
// getModeMeters(GVpumpMode)......: uint16_t   2
// SUMME..........................:           54
// also pro MB ca. 15 Stunden Fahrzeit

// der Compiler berücksichtigt Hardware Optimierung
// deshalb wird sizeof(gpsData) nicht 54 ausgeben, sondern 56 (8-byte boundary).
// Ist man auf genaue Angaben angewiesen, muss das Attribut packed verwendet werden:
// struct __attribute__((packed)) gpsData {...}
// das geht u.a. allerdings auf die Performance, deshalb sorgen wir manuell für die Größe:
#define GPS_RECORD_SIZE 54

struct gpsData
{
  uint32_t datum;   // 4
  uint32_t zeit;    // 4
  double   loclat;  // 8
  double   loclng;  // 8
  float    hdop;    // 4
  float    speed;   // 4 
  float    alt;     // 4
  uint32_t locage;  // 4
  uint32_t oldage;  // 4
  float    dist;    // 4
  float    msp;     // 4 (Meter since pump)
  uint16_t pmode;   // 2
};                 // 54 Bytes

union gpsUnion  // Struktur gpsData wird hier über das char array gelegt
{
  char buffer[GPS_RECORD_SIZE];
  gpsData data;
};

gpsUnion GVgps; // global, no need to allocate each time
const char PROGMEM * GPSLogFile = "/gpslog.txt";

/******************************************
 * setupGPS()
 ******************************************/
void setupGPS(void)
{
  // GPS initialisieren:
  if (!GVmaintenanceMode) {
    DEBUG_OUT.println(F("[setupGPS] Initializing GPS module..."));
    gpsSerial.begin(9600);  // einige GPS Module senden nur mit 4800 baud...
    delay(100);
    if (GVoilerConf.gcf.length() > 0)
    {
      if (!GVoilerConf.gcf.startsWith("/")) GVoilerConf.gcf = "/" + GVoilerConf.gcf;
      if (_FILESYS.exists(GVoilerConf.gcf.c_str()))
      {
        delay(100);
        DEBUG_OUT.println(F("[setupGPS] calling configureGPS()"));
        configureGPS();
      }
      else
      {
        DEBUG_OUT.print(F("[setupGPS] ERROR: GPS Config file not found, check configuration : "));
        DEBUG_OUT.println(GVoilerConf.gcf);
      }
    }
    else DEBUG_OUT.println(F("[setupGPS] no GPS configuration file given"));
  }
  
  // fuer Notlauffunktion vorbereiten
  GVlastGPSData = millis();
}

/************************************************
 * Jedes gelesen Zeichen vom GPS Modul wird hier
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
 * 
 ************************************************************************************/
void checkGPSdata() {
  if (!GVmaintenanceMode)
  {
    while (gpsSerial.available())
    {
      if (evalGPS(gpsSerial.read())) // nur ganze Sätze auswerten
      {
        if (GVoilerConf.gdl) {
          GVoutFile = _FILESYS.open(GPSLogFile, "a");
          GVoutFile.print(GVgpsBuffer);
          GVoutFile.close();
        }

        // nur RMC und GGA Saetze auswerten:
        if (isGGArecord() || isRMCrecord())
        {
          GVmyDisplay.PrintGpsState(GPS_ACTIVE);
          GVlastGPSData = millis();
                    
          // TinyGPSPlus Objekt fuettern:
          for (int i = 0; GVgpsBuffer[i]; i++) GVgpsNew.encode(GVgpsBuffer[i]);
  
          if (isGGArecord()) analyzeGPSInfo();
        }
        GVgpsBufferIndex = 0;
      }
    }
  }
  
  if ((GVlastGPScheck + 1000) < millis()) // nur 1/s aufrufen
  {
    // Wenn überhaupt keine GGA-/RMC-Sätze empfangen wurden: Display Status aktualisieren
    if ((GVlastGPSData + 3000) < millis()) //TODO: Konstante im Code
    {
      GVmyDisplay.PrintGpsState(GPS_NONE);
    }

    // Wenn die letzte Koordinate zu alt, Karenzzeit abgelaufen und KEIN Wartungsmodus, dann simulieren:
    if ((GVoldLocationAge + 3000) < millis() && (GVoilerConf.wts * 1000) < millis() && !GVmaintenanceMode)
    {
      // nach der Karenzzeit und 3s keine GPS-Daten = simulieren
      GVmeterSincePump += GVoilerConf.sim;
    }
    GVlastGPScheck = millis();
  }

  //TODO: check, if this is right here:
  GVmyDisplay.PrintMeter(getModeMeters(GVpumpMode) - GVmeterSincePump);  
  
  // egal, wie oben die Berechnung war, hier die Entscheidung, ob gepumpt werden muss..
  // aber nicht oefter als 1 mal pro Sekunde. Ausserdem funktioniert Dauerpumpen so auch im WartungsMode:
  if (GVpumpMode != MODE_OFF && GVmeterSincePump >= getModeMeters(GVpumpMode))
  {
    //DEBUG_OUT.println(F("[checkGPSdata] calling InitiatePump()"));
    // Erst pumpen, dann Dauerpumpem nach x mal abschalten, wenn sich Moped bewegt...
    if (InitiatePump() && GVpumpMode == MODE_PERMANENT)
    {
      GVoilCounter++;
      if (isMoving() && (GVoilCounter >= MAX_PUMP_ACTION_WHEN_MOVING))
      {
        DEBUG_OUT.println(F("[checkGPSdata] Dauerpumpen wird ausgeschaltet..."));
        setNewMode(MODE_NORMAL);
        // GVoilCounter wird in setNewMode() gesetzt
      }
    }
  }
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

  //DEBUG_OUT.println(F("[analyzeGPSInfo] <<<"));
  if (isUpdated)  // wahr, wenn aktuelle GPS-Daten vorliegen
  {
    //DEBUG_OUT.println(F("[analyzeGPSInfo] gps updated (valid location, i.e. GVgpsNew.location.isUpdated())"));
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
  
  char buffer[9] = "20";

  for (uint8_t i = 1; i < 4; i++)
  {
    buffer[2*i+1] = (date % 10) + '0';
    date /= 10;
    buffer[2*i] = (date % 10) + '0';
    date /= 10;
  }
  buffer[8] = '\0';

  time /= 10000;  // Sekunden und Millisekunden raus...
  GVgpsTrackFilename = String(buffer) + "-" + ((time < 1000) ? "0" + String(time) : String(time));
  DEBUG_OUT.print("date & time: "); DEBUG_OUT.println(GVgpsTrackFilename);
  GVgpsTrackFilename = "/" + GVgpsTrackFilename + ".dat";
 
  return true;
}


/***********************************************
 * Check Movement
 * --------------
 * uint32_t GVlastMovementCheck
 * uint8_t  GVmovementCounter    Anzahl Movements (bei Bewegung ++ (max 30), bei Stillstand -- (min 0))
 * double   GVlastTotalDist      um real Movement zu checken
 * (double GVtotalDist : real gemessene gefahrene Distanz)
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
      DEBUG_OUT.println(F("[checkMovement] Info: Vehicle started moving."));
      GVmyDisplay.PrintMoveState(true); //20200310i
    }
    if (isHalting() && GVlastMovingState)
    {
      GVlastMovingState = false;
      DEBUG_OUT.println(F("[checkMovement] Info: Vehicle STOPPED!"));
      GVmyDisplay.PrintMoveState(false);
    }    
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
  if ((GVlastGPSwrite + ((long)(GVoilerConf.fpw * 1000) - 200L)) > millis())
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

    // alle MIN_MINUTES_FREE nachsehen, ob noch Platz im Filesystem ist - sonst Aufzeichnung einstellen
    if (((millis() - GVlastFScheck) / 60000) > MIN_MINUTES_FREE) 
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
