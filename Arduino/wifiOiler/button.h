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
    DEBUG_OUT.println(F("[setupButton] Button pressed during start.."));
    GVmyLedx.start LED_START_BUTTON_PRESSED;
    DEBUG_OUT.println(F("[setupButton] enter maintenance mode.."));
    //GVmyLedx.printInfo(DEBUG_OUT);
    GVmyLedx.delay();
    while (digitalRead(BUTTON_PIN) == HIGH) yield();  // warten, bis Button losgelassen wird
    GVmyLedx.on(LED_GRUEN);
  } 

  //attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), ButtonISR, CHANGE);
  // mit Interrupts keine saubere Umsetzung hinbekommen... (reine Abfrage ist viel genauer und prellt auch nicht so)
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
    //DEBUG_OUT.println(GVbuttonState == HIGH ? "HIGH" : "Low");
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
          DEBUG_OUT.print(F("toggle WiFi requested..."));
          toggleWiFi();
        }
        else
        {
          DEBUG_OUT.println(F("switching pump mode requested..."));
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
