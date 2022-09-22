/************************************************************************
 * wifiOiler, an automatic distance depending motorbike chain oiler
 * Copyright (C) 2019-2022, volw
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

// this-> wird verwendet zur besseren Lesbarkeit (ist eigentlich nicht notwendig)

#include "oilerDisplay.h"

//********************************************************************

void oilerDisplay::Init(void) {
#ifdef _DISPLAY_AVAILABLE_
  this->begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
  // immer Schriftgröße 2 und weiß
  this->setTextSize(2);
  this->setTextColor(WHITE);
#endif  
}

#ifdef _DISPLAY_AVAILABLE_

void oilerDisplay::Update(void) {
  this->display();
  this->IsUpToDate = true;  
}

uint16_t oilerDisplay::getTextWidth(String s) {
  int16_t x, y;
  uint16_t w, h;
  this->getTextBounds(s, 0, 0, &x, &y, &w, &h);
  return w;
}

void oilerDisplay::ResetToOpScreen(void) {
  this->forceUpdate = true;
  this->InitOpScreen();
  this->PrintModeStr(this->ModeContent);
  this->PrintKmh(this->KmhContent);
  this->PrintMeter(this->MeterContent);
  this->PrintMoveState(this->MoveStateContent);
  this->PrintGpsState(this->GpsStateContent);
  this->PrintFixState(this->FixStateContent);
  this->PrintFileState(this->FileStateContent);
  this->PrintWlanState(this->WlanStateContent);
  this->forceUpdate = false;
  
}

void oilerDisplay::ClearTopBox(void) {
  this->fillRect(TOP_BOX_LEFT, TOP_LINE_TOP, TOP_BOX_WIDTH, TOP_LINE_HEIGHT, BLACK);
}

void oilerDisplay::ClearMeterBox(void) {
  this->fillRect(MIDDLE_RIGHT_BOX_LEFT, MIDDLE_LINE_TOP, MIDDLE_RIGHT_BOX_WIDTH, MIDDLE_LINE_HEIGHT, BLACK);
}

void oilerDisplay::ClearKmhBox(void) {
  this->fillRect(MIDDLE_LEFT_BOX_LEFT, MIDDLE_LINE_TOP, MIDDLE_LEFT_BOX_WIDTH, MIDDLE_LINE_HEIGHT, BLACK);
}

void oilerDisplay::ClearMoveState(void) {
  this->fillRect(BOTTOM_BOX_1_LEFT, BOTTOM_LINE_TOP, BOTTOM_BOX_1_WIDTH, BOTTOM_LINE_HEIGHT, BLACK);
}
void oilerDisplay::ClearGpsState(void) {
  this->fillRect(BOTTOM_BOX_2_LEFT, BOTTOM_LINE_TOP, BOTTOM_BOX_2_WIDTH, BOTTOM_LINE_HEIGHT, BLACK);
}

void oilerDisplay::ClearFixState(void) {
  this->fillRect(BOTTOM_BOX_3_LEFT, BOTTOM_LINE_TOP, BOTTOM_BOX_3_WIDTH, BOTTOM_LINE_HEIGHT, BLACK);
}

void oilerDisplay::ClearFileState(void) {
  this->fillRect(BOTTOM_BOX_4_LEFT, BOTTOM_LINE_TOP, BOTTOM_BOX_4_WIDTH, BOTTOM_LINE_HEIGHT, BLACK);
}

void oilerDisplay::ClearWlanState(void) {
  this->fillRect(BOTTOM_BOX_5_LEFT, BOTTOM_LINE_TOP, BOTTOM_BOX_5_WIDTH, BOTTOM_LINE_HEIGHT, BLACK);
}

#endif
//********************************************************************




void oilerDisplay::Check(void) {
#ifdef _DISPLAY_AVAILABLE_
  if (this->AwaitAcknowledge()) return;

  // auf aktive Meldung checken und ob Zeit abgelaufen ist
  if (this->MessageMillis > 0) {
    if (this->MessageMillis < millis()) { // abgelaufen
      this->MessageOff();
    }
  } else if (!this->IsUpToDate) this->Update();

  if (InvertMillis > 0 && InvertMillis < millis()) {
    this->invertDisplay(false);
    this->InvertMillis = 0;
  }
#endif
}

void oilerDisplay::InitOpScreen(void) {
#ifdef _DISPLAY_AVAILABLE_
  // Grundmuster für Normal Operation Screen erzeugen:
  // --------------------------
  // |                        |
  // --------------------------
  // |         |              |
  // --------------------------
  // |    |    |    |    |    |
  // --------------------------  
  this->clearDisplay();
  // Außenkasten:
  this->drawRect(0,0,this->width(),this->height(), WHITE);
  
  // zwei waagerechte Trennlinien
  this->drawFastHLine(0, TOP_LINE_BOTTOM + 1, this->width(), WHITE);
  this->drawFastHLine(0, MIDDLE_LINE_BOTTOM + 1, this->width(), WHITE);
  
  // horiz. Trennlinie mittlere Zeile
  this->drawFastVLine(MIDDLE_LEFT_BOX_RIGHT + 1, MIDDLE_LINE_TOP, MIDDLE_LINE_HEIGHT, WHITE);

  // restliche Trennlinien untere Zeile
  this->drawFastVLine(BOTTOM_BOX_1_RIGHT + 1, BOTTOM_LINE_TOP, BOTTOM_LINE_HEIGHT, WHITE);
  this->drawFastVLine(BOTTOM_BOX_2_RIGHT + 1, BOTTOM_LINE_TOP, BOTTOM_LINE_HEIGHT, WHITE);
  this->drawFastVLine(BOTTOM_BOX_3_RIGHT + 1, BOTTOM_LINE_TOP, BOTTOM_LINE_HEIGHT, WHITE);
  this->drawFastVLine(BOTTOM_BOX_4_RIGHT + 1, BOTTOM_LINE_TOP, BOTTOM_LINE_HEIGHT, WHITE);

  this->IsUpToDate = false;
#endif
}

void oilerDisplay::PrintModeStr(String modestr) {
#ifdef _DISPLAY_AVAILABLE_
  if (this->ModeContent.equals(modestr) && !this->forceUpdate) return;
  this->ModeContent = modestr;

  if (!this->MessageActive() && !this->AwaitAcknowledge()) {
    this->ClearTopBox();
    this->setCursor(((TOP_BOX_WIDTH - this->getTextWidth(modestr)) / 2) + 1, TOP_PRINT_Y);
    this->print(modestr);
  }
  this->IsUpToDate = false;
#endif
}

void oilerDisplay::PrintKmh(double kmh) {
#ifdef _DISPLAY_AVAILABLE_

  if (this->KmhContent == kmh && !this->forceUpdate) return;
  
  this->KmhContent = kmh;

  if (!this->MessageActive() && !this->AwaitAcknowledge()) {
    String kmhstr = String(kmh < 0 ? 0 : (uint16_t)kmh);
    this->ClearKmhBox();
    this->setCursor(MIDDLE_LEFT_BOX_RIGHT - getTextWidth(kmhstr), MIDDLE_PRINT_Y);
    this->print(kmhstr);
  }
  this->IsUpToDate = false;
#endif  
}

void oilerDisplay::PrintMeter(float meter) {
#ifdef _DISPLAY_AVAILABLE_

  if (this->MeterContent == meter && !this->forceUpdate) return;
  
  this->MeterContent = meter;

  if (!this->MessageActive() && !this->AwaitAcknowledge()) {
    String meterstr = String(meter < 0 ? 0 : (uint16_t)meter);
    
    this->ClearMeterBox();
    this->setCursor(MIDDLE_RIGHT_BOX_RIGHT - getTextWidth(meterstr), MIDDLE_PRINT_Y);
    this->print(meterstr);
  }
  this->IsUpToDate = false;
#endif  
}

void oilerDisplay::PrintMoveState(bool state) {
#ifdef _DISPLAY_AVAILABLE_
  if (this->MoveStateContent == state && !this->forceUpdate) return;
  
  this->MoveStateContent = state;

  if (!this->MessageActive() && !this->AwaitAcknowledge()) {
    this->ClearMoveState();
    if (state) {
      this->setCursor(BOTTOM_BOX_1_PRINT_X, BOTTOM_PRINT_Y);
      this->print("M");
    }
  }
  this->IsUpToDate = false;
#endif  
}

void oilerDisplay::setMaintenanceMode(bool mmode)
{
#ifdef _DISPLAY_AVAILABLE_
  this->MaintenanceMode = mmode;
  PrintGpsState(mmode ? GPS_MAINTENANCE : GPS_NONE);
#endif
}

void oilerDisplay::PrintGpsState(char state) {
#ifdef _DISPLAY_AVAILABLE_

  if (this->MaintenanceMode) state = GPS_MAINTENANCE; // im Wartungsmodus gibt's kein GPS
  
  if (this->GpsStateContent == state && !this->forceUpdate) return;
  
  this->GpsStateContent = state;
  
  if (!this->MessageActive() && !this->AwaitAcknowledge()) {
    this->ClearGpsState();
    this->setCursor(BOTTOM_BOX_2_PRINT_X, BOTTOM_PRINT_Y);
    this->print(state);
  }
  this->IsUpToDate = false;
#endif  
}

void oilerDisplay::PrintFixState(bool state) {
#ifdef _DISPLAY_AVAILABLE_  
  
  if (this->FixStateContent == state && !this->forceUpdate) return;
  
  this->FixStateContent = state;

  if (!this->MessageActive() && !this->AwaitAcknowledge()) {
    this->ClearFixState();
    if (state) {
      this->setCursor(BOTTOM_BOX_3_PRINT_X, BOTTOM_PRINT_Y);
      this->print("F");
    }
  }
  this->IsUpToDate = false;
#endif  
}

void oilerDisplay::PrintFileState(bool state) {
#ifdef _DISPLAY_AVAILABLE_
  
  if (this->FileStateContent == state && !this->forceUpdate) return;

  this->FileStateContent = state;

  if (!this->MessageActive() && !this->AwaitAcknowledge()) {
    this->ClearFileState();
    if (state) {
      this->setCursor(BOTTOM_BOX_4_PRINT_X, BOTTOM_PRINT_Y);
      this->print("T");
    }
  }
  this->IsUpToDate = false;
#endif  
}

/***
 * ' ' = nichts, also aus
 * 'w' = wLan
 * 'A' = Access Point
 */
void oilerDisplay::PrintWlanState(char state) {
#ifdef _DISPLAY_AVAILABLE_
  
  if (this->WlanStateContent == state && !this->forceUpdate) return;

  this->WlanStateContent = state;
  
  if (!this->MessageActive() && !this->AwaitAcknowledge()) {
    this->ClearWlanState();
  
    if (state != WIFI_INACTIVE) {
      this->setCursor(BOTTOM_BOX_5_PRINT_X, BOTTOM_PRINT_Y);
      this->print(state);
    }
  }
  this->IsUpToDate = false;
#endif  
}

void oilerDisplay::PrintMessage(String message, uint32_t duration) {
#ifdef _DISPLAY_AVAILABLE_
  
  // Falls auf Bestätigung gewartet wird, wird keine neue Meldung angezeigt
  if (this->AwaitAcknowledge()) return;
  
  this->clearDisplay();
  this->setCursor(0,0);
  this->print(message);
  MessageMillis = millis() + duration;
  this->display();
#endif  
}

void oilerDisplay::MessageAdd(String message, uint32_t duration) {
#ifdef _DISPLAY_AVAILABLE_
  
  if (this->AwaitAcknowledge()) return;

  if (this->MessageActive()) 
  {
    this->print(message);
    if (duration > 0) MessageMillis = millis() + duration;
    this->display();
  }
#endif  
}

void oilerDisplay::PrintAckMessage(String message) {
#ifdef _DISPLAY_AVAILABLE_
  
  if (this->AwaitAcknowledge()) return;
  
  this->clearDisplay();
  this->setCursor(0,0);
  this->print(message);
  this->MessageMillis = 0;
  waitForButton = true;
  this->display();
#endif  
}

void oilerDisplay::MessageOff(void) {
#ifdef _DISPLAY_AVAILABLE_
  
  if (this->AwaitAcknowledge()) return;

  if (this->MessageMillis > 0) {
    this->MessageMillis = 0;
    this->ResetToOpScreen();
  }
#endif  
}

bool oilerDisplay::MessageActive(void) {
#ifdef _DISPLAY_AVAILABLE_  
  return (MessageMillis > 0);
#else
  return false;
#endif
}

bool oilerDisplay::AwaitAcknowledge() {
#ifdef _DISPLAY_AVAILABLE_  
  return waitForButton; 
#else
  return false;
#endif
};

bool oilerDisplay::Acknowledge() {
#ifdef _DISPLAY_AVAILABLE_
  
  if (AwaitAcknowledge()) {
    waitForButton = false;
    this->ResetToOpScreen();
    return true;
  }
#endif  
  return false;
}

void oilerDisplay::Invert(uint32_t duration) {
#ifdef _DISPLAY_AVAILABLE_
  
  this->InvertMillis = millis() + duration;
  this->invertDisplay(true);
#endif  
}
