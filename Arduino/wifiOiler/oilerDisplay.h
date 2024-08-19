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

#ifndef _OILER_DISPLAY_H_
#define _OILER_DISPLAY_H_

#ifdef _DISPLAY_AVAILABLE_
  #include <Adafruit_SSD1306.h>

  #define SCREEN_WIDTH 128
  #define SCREEN_HEIGHT 64
  #define SSD1306_I2C_ADDRESS 0x3C  // could be 0x3D
  #define OLED_RESET -1   // Reset pin # (or -1 if sharing Arduino reset pin)
  
  #define TOP_PRINT_Y 4
  #define TOP_LINE_TOP 1
  #define TOP_LINE_BOTTOM 20
  #define TOP_LINE_HEIGHT (TOP_LINE_BOTTOM - TOP_LINE_TOP + 1)
  
  #define TOP_BOX_LEFT 1
  #define TOP_BOX_RIGHT 126
  #define TOP_BOX_WIDTH (TOP_BOX_RIGHT - TOP_BOX_LEFT + 1)
  
  #define MIDDLE_PRINT_Y 25
  #define MIDDLE_LINE_TOP 22
  #define MIDDLE_LINE_BOTTOM 41
  #define MIDDLE_LINE_HEIGHT (MIDDLE_LINE_BOTTOM - MIDDLE_LINE_TOP + 1)
  
  #define MIDDLE_LEFT_BOX_LEFT 1
  #define MIDDLE_LEFT_BOX_RIGHT 60
  #define MIDDLE_LEFT_BOX_WIDTH (MIDDLE_LEFT_BOX_RIGHT - MIDDLE_LEFT_BOX_LEFT + 1)
  
  #define MIDDLE_RIGHT_BOX_LEFT 62
  #define MIDDLE_RIGHT_BOX_RIGHT 126
  #define MIDDLE_RIGHT_BOX_WIDTH (MIDDLE_RIGHT_BOX_RIGHT - MIDDLE_RIGHT_BOX_LEFT + 1)
  
  #define BOTTOM_PRINT_Y 46
  #define BOTTOM_LINE_TOP 43
  #define BOTTOM_LINE_BOTTOM 62
  #define BOTTOM_LINE_HEIGHT (BOTTOM_LINE_BOTTOM - BOTTOM_LINE_TOP + 1)
  
  #define BOTTOM_BOX_1_PRINT_X 8
  #define BOTTOM_BOX_1_LEFT 1
  #define BOTTOM_BOX_1_RIGHT 25
  #define BOTTOM_BOX_1_WIDTH (BOTTOM_BOX_1_RIGHT - BOTTOM_BOX_1_LEFT + 1)
  
  #define BOTTOM_BOX_2_PRINT_X 34
  #define BOTTOM_BOX_2_LEFT 27
  #define BOTTOM_BOX_2_RIGHT 50
  #define BOTTOM_BOX_2_WIDTH (BOTTOM_BOX_2_RIGHT - BOTTOM_BOX_2_LEFT + 1)
  
  #define BOTTOM_BOX_3_PRINT_X 59
  #define BOTTOM_BOX_3_LEFT 52
  #define BOTTOM_BOX_3_RIGHT 75
  #define BOTTOM_BOX_3_WIDTH (BOTTOM_BOX_3_RIGHT - BOTTOM_BOX_3_LEFT + 1)
  
  #define BOTTOM_BOX_4_PRINT_X 84
  #define BOTTOM_BOX_4_LEFT 77
  #define BOTTOM_BOX_4_RIGHT 100
  #define BOTTOM_BOX_4_WIDTH (BOTTOM_BOX_4_RIGHT - BOTTOM_BOX_4_LEFT + 1)
  
  #define BOTTOM_BOX_5_PRINT_X 109
  #define BOTTOM_BOX_5_LEFT 102
  #define BOTTOM_BOX_5_RIGHT 126
  #define BOTTOM_BOX_5_WIDTH (BOTTOM_BOX_5_RIGHT - BOTTOM_BOX_5_LEFT + 1)
  
  #define KMH_BUFFER_LENGTH 4
  #define METER_BUFFER_LENGTH 6
#endif

#define WIFI_WLAN 'w'
#define WIFI_ACCESSPOINT 'A'
#define WIFI_INACTIVE ' '
#define GPS_NONE ' '
#define GPS_ACTIVE 'G'
#define GPS_MAINTENANCE 'M'
#define MESSAGE_DURATION 3000
  
#ifdef _DISPLAY_AVAILABLE_
class oilerDisplay : private Adafruit_SSD1306 {
#else
class oilerDisplay {
#endif    
  public:
    #ifdef _DISPLAY_AVAILABLE_
    oilerDisplay() : Adafruit_SSD1306(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,OLED_RESET) {}
    #endif
    void Init(void);   
    void Check(void);  // wenn nötig, Display aktualisieren
    void InitOpScreen(void);
    void PrintModeStr(String modestr);
    void PrintKmh(double kmh);
    void PrintMeter(float meter);
    void PrintMoveState(bool state);
    void PrintGpsState(char state);
    void PrintFixState(bool state);
    void PrintFileState(bool state);
    void PrintWlanState(char state);
    void PrintMessage(String message, uint32_t duration = MESSAGE_DURATION);
    void MessageAdd(String message, uint32_t duration = 0);  // fügt der angezeigten Meldung was hinzu und ändert ggf. Anzeigedauer
    void PrintAckMessage(String message);
    void MessageOff(void);    // schaltet ggf. aktive Meldung aus
    bool MessageActive(void); // true, wenn eine Meldung angezeigt wird
    bool AwaitAcknowledge();
    bool Acknowledge();
    void Invert(uint32_t duration = 1000);
    void setMaintenanceMode(bool mmode = true);
    
#ifdef _DISPLAY_AVAILABLE_    
  private:
    uint32_t MessageMillis = 0;
    uint32_t InvertMillis = 0;
    bool waitForButton = false;
    bool IsUpToDate = true;
    bool forceUpdate = false;
    // Die aktuellen Inhalte werden hier vorgehalten, um z.B. nach Anzeigen anderer Dinge
    // den vorherigen Zustand wiederherzustellen. Außerdem ist so gewährleistet, dass Inhalte
    // im Programm geändert werden können, während z.B. Meldungen angezeigt werden.
    String ModeContent = "";
    double KmhContent = -1;
    float MeterContent = -1;
    bool MoveStateContent = false;
    char GpsStateContent = GPS_NONE;
    bool FixStateContent = false;
    bool FileStateContent = false;
    char WlanStateContent = 0;
    bool MaintenanceMode = false;

    void Update(void);
    void ResetToOpScreen(void); // alte Inhalte werden restored
    uint16_t getTextWidth(String s);
    void ClearTopBox(void);
    void ClearKmhBox(void);
    void ClearMeterBox(void);
    void ClearMoveState(void);
    void ClearGpsState(void);
    void ClearFixState(void);
    void ClearFileState(void);
    void ClearWlanState(void);
#endif    
    
};
#endif
