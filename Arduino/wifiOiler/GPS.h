// gpsNes.date.value()..........: uint32_t   4
// gpsNew.time.value()..........: uint32_t   4
// gpsNew.location.lat()........: double     8
// gpsNew.location.lng()........: double     8
// gpsNew.hdop.value()..........: float      4
// gpsNew.speed.kmph()..........: float      4
// gpsNew.altitude.meters().....: float      4
// gpsNew.location.age()........: uint32_t   4
// millis()-oldLocationAge......: uint32_t   4
// dist.........................: float      4
// meterSincePump...............: float      4
// getModeMeters(pumpMode)......: uint16_t   2
// SUMME........................:           54
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

gpsUnion gps; // global, no need to allocate each time
const char PROGMEM * GPSLogFile = "/gpslog.txt";

/******************************************
 * setupGPS()
 ******************************************/
void setupGPS(void)
{
  // GPS initialisieren:
  if (!maintenanceMode) {
    DEBUG_OUT.println(F("[setupGPS] Initializing GPS module..."));
    gpsSerial.begin(9600);  // einige GPS Module senden nur mit 4800 baud...
    delay(100);
    if (conf.gcf.length() > 0)
    {
      if (!conf.gcf.startsWith("/")) conf.gcf = "/" + conf.gcf;
      if (_FILESYS.exists(conf.gcf.c_str()))
      {
        delay(100);
        DEBUG_OUT.println(F("[setupGPS] calling configureGPS()"));
        configureGPS();
      }
      else
      {
        DEBUG_OUT.print(F("[setupGPS] ERROR: GPS Config file not found, check configuration : "));
        DEBUG_OUT.println(conf.gcf);
      }
    }
    else DEBUG_OUT.println(F("[setupGPS] no GPS configuration file given"));
  }
  
  // fuer Notlauffunktion vorbereiten
  lastGPSData = millis();
}

/************************************************
 * Jedes gelesen Zeichen vom GPS Modul wird hier
 * durchgeschleust.
 * Rueckgabe true, wenn Satzende erreicht wurde.
 ************************************************/
bool evalGPS(char data) {
  gpsBuffer[gpsBufferIndex] = data;
  gpsBuffer[gpsBufferIndex + 1] = '\0';
  // wenn Puffer voll, werden restliche Zeichen verschluckt...
  if (gpsBufferIndex < GPS_BUFFER_LENGTH - 1) gpsBufferIndex++;
  return (data == '\n');
}

/************************************************************************************
 * Lesen von GPS Daten und ggf. Auswertung auslösen
 * Im Wartungsmodus keine Auswertung, um MCU damit nicht zu belasten
 * 
 ************************************************************************************/
void checkGPSdata() {
  if (!maintenanceMode)
  {
    while (gpsSerial.available())
    {
      if (evalGPS(gpsSerial.read())) // nur ganze Sätze auswerten
      {
        if (conf.gdl) {
          outFile = _FILESYS.open(GPSLogFile, "a");
          outFile.print(gpsBuffer);
          outFile.close();
        }

        // nur RMC und GGA Saetze auswerten:
        if (isGGArecord() || isRMCrecord())
        {
          myDisplay.PrintGpsState(GPS_ACTIVE);
          lastGPSData = millis();
                    
          // TinyGPSPlus Objekt fuettern:
          for (int i = 0; gpsBuffer[i]; i++) gpsNew.encode(gpsBuffer[i]);
  
          if (isGGArecord()) analyzeGPSInfo();
        }
        gpsBufferIndex = 0;
      }
    }
  }
  
  if ((lastGPScheck + 1000) < millis()) // nur 1/s aufrufen
  {
    // Wenn überhaupt keine GGA-/RMC-Sätze empfangen wurden: Display Status aktualisieren
    if ((lastGPSData + 3000) < millis()) //TODO: Konstante im Code
    {
      myDisplay.PrintGpsState(GPS_NONE);
    }

    // Wenn die letzte Koordinate zu alt, Karenzzeit abgelaufen und KEIN Wartungsmodus, dann simulieren:
    if ((oldLocationAge + 3000) < millis() && (conf.wts * 1000) < millis() && !maintenanceMode)
    {
      // nach der Karenzzeit und 3s keine GPS-Daten = simulieren
      meterSincePump += conf.sim;
    }
    lastGPScheck = millis();
  }

  //TODO: check, if this is right here:
  myDisplay.PrintMeter(getModeMeters(pumpMode) - meterSincePump);  
  
  // egal, wie oben die Berechnung war, hier die Entscheidung, ob gepumpt werden muss..
  // aber nicht oefter als 1 mal pro Sekunde. Ausserdem funktioniert Dauerpumpen so auch im WartungsMode:
  if (pumpMode != MODE_OFF && meterSincePump >= getModeMeters(pumpMode))
  {
    //DEBUG_OUT.println(F("[checkGPSdata] calling InitiatePump()"));
    // Erst pumpen, dann Dauerpumpem nach x mal abschalten, wenn sich Moped bewegt...
    if (InitiatePump() && pumpMode == MODE_PERMANENT)
    {
      oilCounter++;
      if (isMoving() && (oilCounter >= MAX_PUMP_ACTION_WHEN_MOVING))
      {
        DEBUG_OUT.println(F("[checkGPSdata] Dauerpumpen wird ausgeschaltet..."));
        setNewMode(MODE_NORMAL);
        // oilCounter wird in setNewMode() gesetzt
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
  if (gpsBuffer[0] != '$') return false;
  if (gpsBuffer[1] == 0 || gpsBuffer[2] == 0) return false;
  if (gpsBuffer[3] != 'G' || gpsBuffer[4] != 'G' || gpsBuffer[5] != 'A') return false;
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
  if (gpsBuffer[0] != '$') return false;
  if (gpsBuffer[1] == 0 || gpsBuffer[2] == 0) return false;
  if (gpsBuffer[3] != 'R' || gpsBuffer[4] != 'M' || gpsBuffer[5] != 'C') return false;
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
  bool isUpdated = gpsNew.location.isUpdated();   // benoetige ich unten noch mal

  myDisplay.PrintFixState(isUpdated);
  if (gpsNew.speed.isUpdated()) myDisplay.PrintKmh(gpsNew.speed.kmph());

  //DEBUG_OUT.println(F("[analyzeGPSInfo] <<<"));
  if (isUpdated)  // wahr, wenn aktuelle GPS-Daten vorliegen
  {
    //DEBUG_OUT.println(F("[analyzeGPSInfo] gps updated (valid location, i.e. gpsNew.location.isUpdated())"));
    if (oldLat != 0 || oldLng != 0)  // alte Koordinate vorhanden?
    {
      distance = gpsNew.distanceBetween(oldLat, oldLng, gpsNew.location.lat(), gpsNew.location.lng());
      // Low-Pass Filter:
      if ((distance * 100) <= conf.glf)  // LowPass Filter (in cm)
      {
        distance = 0;   // Simulation: auf der Stelle bleiben
        filter = true;
      } 
      else if (millis() > (oldLocationAge + MAX_OLD_LOCATION_AGE)) // wenn die alte Koordinate zu alt ist..
      {
        // ..wird doch lieber simuliert (ausserhalb Karenzzeit):
        distance = simMeters();
      }
      else  // hier hat eine "echte" Bewegung stattgefunden:
      {
        totalDist += distance;
        lastMoveMillis = millis();
      }
    }
    else  // simulieren:
    {
      distance = simMeters();
    }
    if (!filter)  // sonst wird praktisch Verbleib auf alter Koordinate simuliert
    {
      oldLat = gpsNew.location.lat();
      oldLng = gpsNew.location.lng();  // nach Abruf ist isUpdated() unbrauchbar
    }
    oldLocationAge = millis();
  }
// hier keine Simulation nötig, wird an aufrufender Stelle gemacht...
  meterSincePump += distance;
  
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
  gpsTrackFilename = String(buffer) + "-" + ((time < 1000) ? "0" + String(time) : String(time));
  DEBUG_OUT.print("date & time: "); DEBUG_OUT.println(gpsTrackFilename);
  gpsTrackFilename = "/" + gpsTrackFilename + ".dat";
 
  return true;
}


/***********************************************
 * Check Movement
 * --------------
 * uint32_t lastMovementCheck
 * uint8_t  movementCounter    Anzahl Movements (bei Bewegung ++ (max 30), bei Stillstand -- (min 0))
 * double   lastTotalDist      um real Movement zu checken
 * (double totalDist : real gemessene gefahrene Distanz)
 * Jede Sekunde wird geprüft, ob sich die total gefahrene (nur per GPS gemessene) Strecke geändert hat.
 * Wenn das so ist, wird ein Zähler erhöht (bis max. MAX_MOVEMENT_COUNT, derzeit 30).
 * Ist dieser Zählerstand also 30, bedeutet dies, dass sich die Strecke 
 * in den letzten 30 Sekunden sekündlich geändert hat -> die Karre also rollt!!
 ***********************************************/ 
void checkMovement()
{
  if (((conf.wts * 1000) < millis()) && ((lastMovementCheck + 1000) < millis()))
  {
    if (lastTotalDist != totalDist) // es hat eine Bewegung stattgefunden
    {
      if (movementCounter < MAX_MOVEMENT_COUNT) movementCounter++;
      lastTotalDist = totalDist;
    }
    else  // keine Bewegung
    {
      if (movementCounter > 0) movementCounter--;
    }
    lastMovementCheck = millis();
    
    // Hier noch eine Ausgabe für's Log:
    if (isMoving() && !lastMovingState)
    {
      lastMovingState = true;
      DEBUG_OUT.println(F("[checkMovement] Info: Vehicle started moving."));
      myDisplay.PrintMoveState(true); //20200310i
    }
    if (isHalting() && lastMovingState)
    {
      lastMovingState = false;
      DEBUG_OUT.println(F("[checkMovement] Info: Vehicle STOPPED!"));
      myDisplay.PrintMoveState(false);
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
  if (currentfpw == 0) 
  {
    if (!myLedx.isActive()) myLedx.start LED_GPS_WRITE_NO_TRACKS;
    myDisplay.PrintFileState(false);
    return;  // bei 0 keine Tracks...
  }
  
  if (!validGPSFilename)  // es gibt noch keinen Dateinamen - aber so gut wie sicher Datum und Uhrzeit...
  {
    // return == false, falls Datum noch 0 war...
    validGPSFilename = createDateFilename(gpsNew.date.value(), gpsNew.time.value());
  }

  // wenn nur alle paar Sekunden geschrieben wird, sollte der dist Wert aufkumuliert werden 
  cumDist += dist;

  if (!validGPSFilename)
  {
    myDisplay.PrintFileState(false);
    return;
  }
  if ((lastGPSwrite + ((long)(conf.fpw * 1000) - 200L)) > millis())
    return; 

  outFile = _FILESYS.open(gpsTrackFilename, "a");
  if (outFile) {
    gps.data.datum  = gpsNew.date.isValid() ? gpsNew.date.value() : 0;
    gps.data.zeit   = gpsNew.time.isValid() ? gpsNew.time.value() : 0;
    if (gpsNew.location.isValid()) {
      gps.data.loclat = gpsNew.location.lat();
      gps.data.loclng = gpsNew.location.lng();
    } else {
      gps.data.loclat = 0;
      gps.data.loclng = 0;
    }
    gps.data.hdop   = (float)(gpsNew.hdop.isValid() ? gpsNew.hdop.value() : 0);
    gps.data.speed  = (float)(gpsNew.speed.isValid() ? gpsNew.speed.kmph() : 0);
    gps.data.alt    = (float)(gpsNew.altitude.isValid() ? gpsNew.altitude.meters() : 0);
    gps.data.locage = gpsNew.location.age();
    gps.data.oldage = (millis()-oldLocationAge);
    gps.data.dist   = cumDist;
    gps.data.msp    = meterSincePump;
    gps.data.pmode  = getModeMeters(pumpMode);
  
    outFile.write((uint8_t*)gps.buffer, sizeof(gps.buffer));
    outFile.close();
    lastGPSwrite = millis();
    cumDist = 0;

    // alle MIN_MINUTES_FREE nachsehen, ob noch Platz im Filesystem ist - sonst Aufzeichnung einstellen
    if (((millis() - lastFScheck) / 60000) > MIN_MINUTES_FREE) 
    {
      checkFilesystemSpace();
    }
    if (!myLedx.isActive()) myLedx.start LED_GPS_WRITE_SUCCESS;
    totalGPSwrite++;
    myDisplay.PrintFileState(true);
  }
  else // error writing to file
  {
    if (!myLedx.isActive()) myLedx.start LED_GPS_WRITE_FAILED;
    totalWriteErrors++;
    myDisplay.PrintFileState(false);
  }
}
