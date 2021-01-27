/****************************************************
 * Setzen des neuen Modus für Pumpensteuerung
 * Aufruf durch Button handling oder Web-Interface
 ****************************************************/
void setNewMode(tPumpMode newMode)
{
  //20200310d if (newMode < MODE_OFF || newMode > MODE_PERMANENT) newMode = MODE_NORMAL;
  if (newMode < MODE_OFF || newMode > MODE_PERMANENT) newMode = MODE_OFF;

  // da bei MODE_PERMANENT der "Öl-zähler" nicht aktualisiert wird,
  // um die Anzahl der Schreibvorgänge im Filesystem zu begrenzen,
  // hier jetzt nachholen, bevor er neu gesetzt wird:
  if (pumpMode == MODE_PERMANENT) {
    #ifndef _NO_PUMP_ 
      conf.updateOilCounter();
    #endif    
    oilCounter = 0;
  }

  DEBUG_OUT.print(F("New pump mode is >"));
  DEBUG_OUT.print(getPumpModeStr(newMode));
  DEBUG_OUT.print(F("< - meters til pump: "));
  pumpMode = newMode;
  if (newMode == MODE_OFF)
    DEBUG_OUT.println(F("(Pumpe deaktiviert)"));
  else if (getModeMeters(pumpMode) < meterSincePump)
    DEBUG_OUT.println(0);
  else
    DEBUG_OUT.println(getModeMeters(pumpMode) - meterSincePump);
  
  myDisplay.PrintModeStr(getPumpModeStr(pumpMode));
  //LED Response einleiten:
  if (newMode == MODE_OFF) myLedx.start LED_SWITCH_MODE_OFF;
  else myLedx.start LED_SWITCH_MODE_RESPONSE;
}

/**************************************************
 * check oil: conf.otk < conf.use ?? 
 **************************************************/
void checkTank(void)
{
  // Prüfen, ob Tank leer:
  if (conf.otk <= conf.use)
  {
    myDisplay.PrintAckMessage("Refill\noil tank +");  //20200310i
    // dann ein bisschen Theater machen:
    DEBUG_OUT.println(F("Tank leer? - Öl nachfüllen und Zähler zurücksetzen (Konfiguration)"));
    myLedx.add(LED_GRUEN, 175);
    myLedx.add(LED_ROT, 150);
    myLedx.start(10);
    myLedx.delay();
  }
}

/**************************************************
 * returns number of meters, depending on the mode
 **************************************************/
uint16_t getModeMeters(tPumpMode mode)
{
  switch (mode) {
    case MODE_NORMAL: 
      return conf.nmm;
    case MODE_REGEN: 
      return conf.rmm;
    case MODE_OFFROAD: 
      return conf.omm;
  }
  // bei MODE_OFF und MODE_PERMANENT:
  return 0; 
}

/***************************************************
 * Anzahl Meter, die simuliert werden, wenn kein GPS
 * Signal empfangen wird. Innerhalb der Karenzeit
 * (gemessen ab Start des Ölers = conf.wts) wird nicht
 * simuliert, also 0 zurückgegeben.
 **************************************************/
uint8_t simMeters()
{
  return (millis() > (conf.wts * 1000UL)) ? conf.sim : 0;
}

/**************************************************
 * Ausgabe string zum übergebenen pumpMode
 */
String getPumpModeStr(tPumpMode nMode) {
  if (nMode == MODE_OFF)       return(F("AUS"));
  if (nMode == MODE_NORMAL)    return(F("Normal"));
  if (nMode == MODE_REGEN)     return(F("Regen"));
  if (nMode == MODE_OFFROAD)   return(F("Offroad"));
  if (nMode == MODE_PERMANENT) return(F("Permanent"));
  return(F("UNKNOWN"));
}

/***********************************************
 * isMoving: true, wenn in den letzten 
 * MAX_MOVEMENT_COUNT Sekunden mindestens
 * MIN_MOVEMENT_POSITIV Bewegungen stattgefunden haben
***********************************************/
bool isMoving()
{
  return (movementCounter > MIN_MOVEMENT_POSITIV);
}

/***********************************************
 * isHalting: true, wenn in den letzten 
 * MAX_MOVEMENT_COUNT Sekunden weniger als
 * MAX_MOVEMENT_NEGATIVE Bewegungen stattgefunden haben
***********************************************/
bool isHalting()
{
  return (movementCounter < MAX_MOVEMENT_NEGATIVE);
}

/***********************************
 * Oelversorgung (Pumpen) einleiten
 ***********************************/
bool InitiatePump() {

  #ifdef _NO_PUMP_ 
    return true;
  #endif
  if (myPumpx.isActive()) return false;

  // innerhalb der ersten 1,5 Sekunden nach Button Press soll nicht gleich gepumpt werden
  // damit bei Ziel 'Aus' über 'Permanent' nicht gleich gepumpt wird.
  if ((lastPressed + 1500) > millis()) return false;

  DEBUG_OUT.print(F("[InitiatePump] pumpen nach (s): "));
  DEBUG_OUT.print(millis() / 1000);
  DEBUG_OUT.print(F(", pump mode is >"));
  DEBUG_OUT.print(getPumpModeStr(pumpMode));    
  DEBUG_OUT.println(F("<"));
  totalPumpCount += conf.pac;
  conf.use += conf.pac;
  // bei Dauerpumpen keine Aktualisierung (erst danach)
  if (pumpMode != MODE_PERMANENT) {
    if (!conf.updateOilCounter()) {
      DEBUG_OUT.println(F("[InitiatePump] Fehler beim Öffnen der OilCounter Datei"));
    }
  }
  myPumpx.start PUMP_ACTION;
  myLedx.start LED_SHOW_PUMP_ACTION;
  myDisplay.Invert(1500);
  
  // vor Aufruf wird bereits geprüft, ob meterSincePump größer als conf.getModeMeters(pumpMode) ist
  // (obige Aussage stimmt nicht: bei Auslösen Pumpentest über Menü wird nicht geprüft!!!)
  meterSincePump = 0;
  return true;
}
