#include "biLedx.h"

// p1: Pin, der geschaltet werden soll
// p2: Pin, der geschaltet werden soll (0=kein zweiter)
// on: Signal, das den Pin "anschaltet", 
//     default = HIGH, Gegenteil autom. LOW
biLedx::biLedx(uint8_t p1, uint8_t p2, uint8_t on)
{
  pin1 = p1;
  pin2 = p2;
  if (on != onState)
  {
    offState = onState;
    onState = on;
  }
  if (pin1 != 0) pinMode(pin1, OUTPUT);
  if (pin2 != 0) pinMode(pin2, OUTPUT);
}


// einfachen Schaltbefehle hinzufügen:
// p: Pin, d:Dauer in ms
void biLedx::add(uint8_t p, uint16_t d)
{ 
  aBlink.push_back({ p, d }); 
}

// 1 oder mehrere Blinkbefehle hinzuügen
// p: Pin, dan: Dauer an in ms, daus:Dauer aus in ms, w:Wiederholungen (default=1)
void biLedx::add(uint8_t p, uint16_t dan, uint16_t daus, uint16_t w) // mit An-/Auszeiten, w=Wiederholungen
{
  for (uint16_t i = 0; i < w; i++)
  {
    add(p, dan);
    add(0, daus);
  }
}

// alles ausschalten, auf inaktiv setzen und Queue 
// für Schaltbefehle zurücksetzen (löschen)
void biLedx::reset() 
{ 
  index = NOT_ACTIVE;
  aBlink.clear(); 
  priv_off();
}


bool biLedx::isActive() 
{ 
  return (index > NOT_ACTIVE); 
}


void biLedx::start(uint16_t w)
{
  if (w < 1) return;
  if (aBlink.size() == 0) return;
  //printInfo(Serial);
  index = 0;
  loopCount = w - 1;
  timer = millis();
  priv_on(aBlink[index].pin);
}


void biLedx::start(uint8_t p, uint16_t dan, uint16_t daus, uint16_t w)
{
  reset();
  add(p, dan);
  if (daus > 0) add(0, daus);
  start(w);
}


void biLedx::delay()
{
  while (isActive()) {
    yield();
    checkState();
  }
}


void biLedx::checkState()
{
  if (!isActive()) return;
  uint32_t tempMillis = millis();
//  Serial.print(timer);
//  Serial.print(" + ");
//  Serial.print(aBlink[index].dauer);
//  Serial.print("(");
//  Serial.print(index);
//  Serial.print(") = ");
//  Serial.print(timer + (uint32_t)aBlink[index].dauer);
//  Serial.print(" <= ");
//  Serial.print(tempMillis);
//  Serial.print(" = ");
//  Serial.println((timer + (uint32_t)aBlink[index].dauer) <= tempMillis);
  if ((timer + (uint32_t)aBlink[index].dauer) <= tempMillis)
  {
    //printInfo(Serial);
    // nächsten Schaltvorgang einleiten
    index++;
    // Abfrage etwas umständlicher (aber so sicher) da unsigned mit signed verglichen wird
    if (index >= 0 && static_cast<uint16_t>(index) >= aBlink.size())
    {
      if (loopCount > 0) {  // auf in die nächste Runde
        loopCount--;
        index = 0;
      } else {
        reset(); // Ende allen Schaltens
        return;
      }
    }
    priv_on(aBlink[index].pin);  // bei Pin=0 wird ausgeschaltet
    timer = millis();
  }
}


void biLedx::on(uint8_t p)
{ 
  reset();  // falls loop aktiv, wird diese zurückgesetzt
  priv_on(p);
}


void biLedx::off() 
{ 
  reset();
}

void biLedx::priv_on(uint8_t p)
{ 
  priv_off();
  if (p == 0) return;
  if (p == DEFAULT_PIN) p = pin1;
  // Abfrage der Pins zur Sicherheit, damit nicht versehentlich andere Pins geschaltet werden:
  if (p == pin1 || p == pin2)
    digitalWrite(p, onState);
}


void biLedx::priv_off() 
{ 
  if (pin1 != 0) digitalWrite(pin1, offState);
  if (pin2 != 0) digitalWrite(pin2, offState);
}


//void biLedx::printInfo(HardwareSerial &serOut)
//{
//  serOut.print(F("index = ")); serOut.println(index);
//  serOut.print(F("timer = ")); serOut.println(timer);
//  serOut.print(F("loopC = ")); serOut.println(loopCount);
//  serOut.print(F("pin1  = ")); serOut.println(pin1);
//  serOut.print(F("pin2  = ")); serOut.println(pin2);
//  serOut.print(F("onSta = ")); serOut.println(onState);
//  serOut.print(F("offSt = ")); serOut.println(offState);
//  for (uint8_t i = 0; i < aBlink.size(); i++)
//  {
//    serOut.print(i);
//    serOut.print(" : pin=");
//    serOut.print(aBlink[i].pin);
//    serOut.print(", dauer=");
//    serOut.print(aBlink[i].dauer);
//    serOut.println();
//  }
//}
