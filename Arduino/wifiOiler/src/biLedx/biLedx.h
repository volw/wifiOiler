/*****************************
Usage: mehrere Möglichkeiten (vorausgesetzt LED_PIN_ROT und LED_PIN_GRUEN sind definiert (bei einer zweifarbigen LED z.B.)

1. Lichtsignale einzeln zusammenstellen und dann auslösen (blinkt in der u.a. Reihenfolge)
    biLed.add(LED_PIN_GRUEN, 300);      // 300ms grün an
    biLed.add(LED_PIN_ROT, 500);        // 500ms rot an (ohne Pause zum Grünlicht zuvor)
    biLed.add(0, 200);                  // 200ms aus
    biLed.add(LED_PIN_ROT, 300);        // 300ms rot an
    biLed.add(LED_PIN_GRUEN, 500);      // 500ms grün an (ohne Pause)
  biLed.add(0, 200);                    // 200ms aus
  biLed.add(LED_PIN_ROT, 500, 300, 5);  // 5-maliges Blinken (rot) hinzufügen (500ms an und 300ms aus)
  biLed.start(5);                       // komplette obere Folge auslösen und zwar 5 mal hintereinander
  biLed.delay();                        // warten, bis alle Signale vollständig abgearbeitet wurden (mit diesem Beispiel 6 Sekunden)

2. biLed.add(LED_PIN_GRUEN, 500, 300);  // 500ms grün, danach 300ms aus
   biLed.start(5);                      // 5 mal loopen lassen: blinkt 5 mal grün mit Dauer von 500ms und 300ms Pause dazwischen

3. biLed.start(LED_PIN_GRUEN, 500, 300, 5); // genau wie 2 nur mit einem Befehl

******************************/
#ifndef __BI_LEDX__H__
#define __BI_LEDX__H__
#include <arduino.h>

#define NOT_ACTIVE -1
#define DEFAULT_PIN 255

struct blink
{
  uint8_t pin;  // 0 = aus
  uint16_t dauer; // max. 65535
};

class biLedx
{
  public:
    /***
      Deklaration 
    */
    biLedx(uint8_t p1, uint8_t p2 = 0, uint8_t on = HIGH);
    ~biLedx() {};
    // EIN Schaltvorgang hinzufügen:
    void add(uint8_t p, uint16_t d);
    // 1-n EIN/AUS Schaltvorgänge hinzufügen:
    void add(uint8_t p, uint16_t dan, uint16_t daus, uint16_t w = 1);
    void start(uint16_t w = 1); // vorh. Loop x-mal starten, default = 1
    // kombinierte add() - start() Möglichkeiten:
    void start(uint8_t p, uint16_t dan, uint16_t daus=0, uint16_t w = 1);
    // wartet, bis alle Blinksignale abgearbeitet sind:
    void delay();
    void reset();
    bool isActive();
    void checkState();
    void on(uint8_t p = DEFAULT_PIN);
    void off();
    //void printInfo(HardwareSerial &serOut);

  private:
    //void init(uint8_t p1, uint8_t on);
    // eigenen An- und Auschaltfunktionen, da obige Fkt.
    // einen reset der Queue auslösen (müssen)
    void priv_on(uint8_t p = DEFAULT_PIN);
    void priv_off();
    int16_t index = NOT_ACTIVE;
    unsigned long timer;
    uint16_t loopCount = 0;
    uint8_t pin1 = 0;
    uint8_t pin2 = 0;
    uint8_t onState = HIGH;
    uint8_t offState = LOW;
    std::vector<blink> aBlink;  // Queue für Blinksignale
};
#endif // !__BI_LEDX__H__
