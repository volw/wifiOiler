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
    maintenanceMode = true;
    myDisplay.setMaintenanceMode(true);
    DEBUG_OUT.println(F("[setupButton] Button pressed during start.."));
    myLedx.start LED_START_BUTTON_PRESSED;
    DEBUG_OUT.println(F("[setupButton] enter maintenance mode.."));
    //myLedx.printInfo(DEBUG_OUT);
    myLedx.delay();
    while (digitalRead(BUTTON_PIN) == HIGH) yield();  // warten, bis Button losgelassen wird
    myLedx.on(LED_GRUEN);
  } 

  // Button initialisieren
  pinMode(BUTTON_PIN, INPUT);
  //attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), ButtonISR, CHANGE);
  // mit Interrupts keine saubere Umsetzung hinbekommen... (reine Abfrage ist viel genauer und prellt auch nicht so)
  delay(50);     // kurze Zeit zum "Setzen"
}

/**********************************************************************
 * Hier wird überprüft, ob der Button gedrückt oder losgelassen wurde.
 * Anhand der Differenz zwischen Druck und loslassen wird entschieden,
 * ob der Button kurz oder lang gedrückt wurde.
 **********************************************************************/
void checkButton()
{
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState != lastButtonState)
  {
    //DEBUG_OUT.println(buttonState == HIGH ? "HIGH" : "Low");
    if (buttonState == HIGH)
    {
      if (millisPressed == 0) millisPressed = millis();
    }
    else if (millisPressed > 0)
    {
      // noch ein wenig debouncen:
      if (lastPressed + DEBOUNCE_MS < millis())
      {
        if (millisPressed + BUTTON_LONG_DURATION < millis())
        {
          DEBUG_OUT.print(F("toggle WiFi requested..."));
          toggleWiFi();
        }
        else
        {
          DEBUG_OUT.println(F("switching pump mode requested..."));
          if (!myDisplay.Acknowledge())
            setNewMode((tPumpMode)(pumpMode + 1)); // check in setNewMode()    
        }
        lastPressed = millis();
      }
      millisPressed = 0;
    }
    lastButtonState = buttonState;
  }
  else if (buttonState == HIGH && millisPressed > 0 && millisPressed + BUTTON_LONG_DURATION < millis())
  {
    myLedx.on(LED_ROT);
  }
}
