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
