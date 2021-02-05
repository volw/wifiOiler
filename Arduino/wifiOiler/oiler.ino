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
  if (GVpumpMode == MODE_PERMANENT) {
    #ifndef _NO_PUMP_ 
      GVoilerConf.updateOilCounter();
    #endif    
    GVoilCounter = 0;
  }

  DEBUG_OUT.printf(PSTR(MSG_DBG_NEW_PUMP_MODE), getPumpModeStr(newMode).c_str());
  GVpumpMode = newMode;
  if (newMode == MODE_OFF)
    DEBUG_OUT.println(F(MSG_DBG_PUMP_DEACTIVATED));
  else if (getModeMeters(GVpumpMode) < GVmeterSincePump)
    DEBUG_OUT.println(0);
  else
    DEBUG_OUT.println(getModeMeters(GVpumpMode) - GVmeterSincePump);
  
  GVmyDisplay.PrintModeStr(getPumpModeStr(GVpumpMode));
  //LED Response einleiten:
  if (newMode == MODE_OFF) GVmyLedx.start LED_SWITCH_MODE_OFF;
  else GVmyLedx.start LED_SWITCH_MODE_RESPONSE;
}

/**************************************************
 * check oil: GVoilerConf.otk < GVoilerConf.use ?? 
 **************************************************/
void checkTank(void)
{
  // Prüfen, ob Tank leer:
  if (GVoilerConf.otk <= GVoilerConf.use)
  {
    GVmyDisplay.PrintAckMessage("Refill\noil tank +");  //20200310i
    // dann ein bisschen Theater machen:
    DEBUG_OUT.println(F(MSG_DBG_OIL_TANK_EMPTY));
    GVmyLedx.add(LED_GRUEN, 175);
    GVmyLedx.add(LED_ROT, 150);
    GVmyLedx.start(10);
    GVmyLedx.delay();
  }
}

/**************************************************
 * returns number of meters, depending on the mode
 **************************************************/
uint16_t getModeMeters(tPumpMode mode)
{
  switch (mode) {
    case MODE_NORMAL: 
      return GVoilerConf.nmm;
    case MODE_REGEN: 
      return GVoilerConf.rmm;
    case MODE_OFFROAD: 
      return GVoilerConf.omm;
  }
  // bei MODE_OFF und MODE_PERMANENT:
  return 0; 
}

/***************************************************
 * Anzahl Meter, die simuliert werden, wenn kein GPS
 * Signal empfangen wird. Innerhalb der Karenzeit
 * (gemessen ab Start des Ölers = GVoilerConf.wts) wird nicht
 * simuliert, also 0 zurückgegeben.
 **************************************************/
uint8_t simMeters()
{
  return (millis() > (GVoilerConf.wts * 1000UL)) ? GVoilerConf.sim : 0;
}

/**************************************************
 * Ausgabe string zum übergebenen pumpMode
 * Strings must match div ids in html (pumpmode.htm)
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
  return (GVmovementCounter > MIN_MOVEMENT_POSITIV);
}

/***********************************************
 * isHalting: true, wenn in den letzten 
 * MAX_MOVEMENT_COUNT Sekunden weniger als
 * MAX_MOVEMENT_NEGATIVE Bewegungen stattgefunden haben
***********************************************/
bool isHalting()
{
  return (GVmovementCounter < MAX_MOVEMENT_NEGATIVE);
}

/***********************************
 * Oelversorgung (Pumpen) einleiten
 ***********************************/
bool TriggerPump() {

  #ifdef _NO_PUMP_ 
    return true;
  #endif
  if (GVmyPumpx.isActive()) return false;

  // innerhalb der ersten 1,5 Sekunden nach Button Press soll nicht gleich gepumpt werden
  // damit bei Ziel 'Aus' über 'Permanent' nicht gleich gepumpt wird.
  if ((GVlastPressed + 1500) > millis()) return false;

  DEBUG_OUT.printf(PSTR(MSG_DBG_TRIGGER_PUMP), int(millis()/1000), getPumpModeStr(GVpumpMode).c_str());
 
  GVtotalPumpCount += GVoilerConf.pac;
  GVoilerConf.use += GVoilerConf.pac;
  // bei Dauerpumpen keine Aktualisierung (erst danach)
  if (GVpumpMode != MODE_PERMANENT) {
    if (!GVoilerConf.updateOilCounter()) {
      DEBUG_OUT.println(F(MSG_DBG_ERROR_OPEN_OILCNT_FILE));
    }
  }
  GVmyPumpx.start PUMP_ACTION;
  GVmyLedx.start LED_SHOW_PUMP_ACTION;
  GVmyDisplay.Invert(1500);
  
  // vor Aufruf wird bereits geprüft, ob GVmeterSincePump größer als GVoilerConf.getModeMeters(GVpumpMode) ist
  // (obige Aussage stimmt nicht: bei Auslösen Pumpentest über Menü wird nicht geprüft!!!)
  GVmeterSincePump = 0;
  return true;
}
