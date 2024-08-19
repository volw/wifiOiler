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

/**********************************************
 * setupButton()
 **********************************************/ 
void setupButton(void)
{
// Interne PullUps koennen ueber mit laengeren Kabeln verbundenen Tastern
// unzuverlaessig werden - dann ggf. ext. Pull-up verwenden
// s. http://www.gammon.com.au/forum/?id=11955
  pinMode(BUTTON_PIN, INPUT);
  if (digitalRead(BUTTON_PIN) == HIGH)
  {
    GVmaintenanceMode = true;
    GVmyDisplay.setMaintenanceMode(true);
    debugPrintf(PSTR(MSG_DBG_BUTTON_PRESSED_ON_START));
    GVmyLedx.start LED_START_BUTTON_PRESSED;
    infoPrintf(PSTR(MSG_DBG_ENTER_MAINTENANCE_MODE));
    //GVmyLedx.printInfo(Serial);
    GVmyLedx.delay();
    while (digitalRead(BUTTON_PIN) == HIGH) yield();  // warten, bis Button losgelassen wird
    GVmyLedx.on(LED_GRUEN);
  } 

  //attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), ButtonISR, CHANGE);
  // mit Interrupts keine saubere Umsetzung hinbekommen... 
  // (reine Abfrage ist viel genauer und prellt auch nicht so)
}

/**********************************************************************
 * Hier wird überprüft, ob der Button gedrückt oder losgelassen wurde.
 * Anhand der Differenz zwischen Druck und loslassen wird entschieden,
 * ob der Button kurz oder lang gedrückt wurde.
 **********************************************************************/
void checkButton()
{
  GVbuttonState = digitalRead(BUTTON_PIN);
  if (GVbuttonState != GVlastButtonState)
  {
    debugPrintf(PSTR(MSG_DBG_BUTTON_STATE), GVbuttonState == HIGH ? "HIGH" : "Low");
    if (GVbuttonState == HIGH)
    {
      if (GVmillisPressed == 0) GVmillisPressed = millis();
    }
    else if (GVmillisPressed > 0)
    {
      // noch ein wenig debouncen:
      if (GVlastPressed + DEBOUNCE_MS < millis())
      {
        if (GVmillisPressed + BUTTON_LONG_DURATION < millis())
        {
          debugPrintf(PSTR(MSG_DBG_BUTTON_LONG_PRESSED));
          toggleWiFi();
        }
        else
        {
          debugPrintf(PSTR(MSG_DBG_BUTTON_SHORT_PRESSED));
          if (!GVmyDisplay.Acknowledge())
            setNewMode((tPumpMode)(GVpumpMode + 1)); // check in setNewMode()    
        }
        GVlastPressed = millis();
      }
      GVmillisPressed = 0;
    }
    GVlastButtonState = GVbuttonState;
  }
  else if (GVbuttonState == HIGH && GVmillisPressed > 0 && GVmillisPressed + BUTTON_LONG_DURATION < millis())
  {
    GVmyLedx.on(LED_ROT);
  }
}
