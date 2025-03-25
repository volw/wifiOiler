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

#pragma once

#ifndef __DONT_USE_TIME__
  #define __USE_TIME__
#endif

#include <arduino.h>
#include <LittleFS.h>

#ifdef __USE_TIME__
  #include <time.h>
  #include <TZ.h>

  /* Configuration of NTP */
  #define MY_NTP_SERVER "de.pool.ntp.org"
  #define MY_TZ TZ_Europe_Berlin
#endif

// ********************* some logging Makros ************************************
#define _LOG_CRITICAL_ 0  // Fehler, der eine weitere Bearbeitung unmöglich macht
#define _LOG_ERROR_    1  // Fehler, der stört aber grundsätzlich die Funktionalität weiter ermöglicht
#define _LOG_WARN_     2  // es kann weiter gehen - aber es könnte besser sein
#define _LOG_INFO_     3  // einfach info
#define _LOG_DEBUG_    4  // detaillierte Info
// die geschweiften Klammern sind hier wichtig, da sonst Einzelanweisungen falsch interpretiert werden,
// also nicht als Block, z.B. bei     if (condition) logPrintf(...);
#define criticalPrintf(...) if(StaticLogger.getLogLevel()>=_LOG_CRITICAL_){StaticLogger.printf("[%s] ",__FUNCTION__);StaticLogger.printf(__VA_ARGS__);}
#define errorPrintf(...)    if(StaticLogger.getLogLevel()>=_LOG_ERROR_)   {StaticLogger.printf("[%s] ",__FUNCTION__);StaticLogger.printf(__VA_ARGS__);}
#define warnPrintf(...)     if(StaticLogger.getLogLevel()>=_LOG_WARN_)    {StaticLogger.printf("[%s] ",__FUNCTION__);StaticLogger.printf(__VA_ARGS__);}
#define infoPrintf(...)     if(StaticLogger.getLogLevel()>=_LOG_INFO_)    {StaticLogger.printf("[%s] ",__FUNCTION__);StaticLogger.printf(__VA_ARGS__);}
#define debugPrintf(...)    if(StaticLogger.getLogLevel()>=_LOG_DEBUG_)   {StaticLogger.printf("[%s] ",__FUNCTION__);StaticLogger.printf(__VA_ARGS__);}

#define logPrintf(...)      StaticLogger.log(__FUNCTION__, __VA_ARGS__);

class LittleFSlogger: public Print {
  protected:
    String filename;
    File logfile;
    bool usefile = false;
    bool newline = true;
    bool toSerial = true;
    uint8_t logLevel = _LOG_WARN_;

  public:

    bool begin(String fname = "", bool toSer = true) {
      // if toSer is true, logger will print to Serial as well

      #ifdef __USE_TIME__
        configTime(MY_TZ, MY_NTP_SERVER); // important for local time!!
      #endif

      filename = fname;
      usefile = (filename.length() > 0);

      toSerial = toSer;

      if (logfile) logfile.close();
      if (usefile) {
        logfile = LittleFS.open(filename, "a");
        return ((bool)logfile);
      }
      return true;
    }

    void setLogLevel(uint8_t _logLevel){
      logLevel = _logLevel;
    }

    uint8_t getLogLevel(){
      return logLevel;
    }

    void flush(void){
      if (usefile && (bool)logfile) logfile.flush();
      if (toSerial) Serial.flush();
    }

    void close(void) {
      if (logfile) logfile.close();
    }

    void beforeWrite() {
//        Serial.println("[beforeWrite]");
        if (usefile && !(bool)logfile) begin(filename, toSerial);
        if (newline) {
          newline = false;
          #ifdef __USE_TIME__
            this->printf("%s:", this->getDateTimeString());
          #else
            this->printf("%d:", millis());
          #endif
        }
    }

    size_t write(uint8_t c) override
    {
//        Serial.println("[write(uint8_t c)]");
        beforeWrite();
        newline = (c == '\n');
        if (toSerial) Serial.write(c);
        if (usefile)
        {
          logfile.write(c);
          if (newline) logfile.flush();
        }
        return 1;
    }

    size_t write(const uint8_t *buffer, size_t size) override
    {
        size_t n = 0;
        for (size_t i = 0; i < size; i++) n += write((uint8_t)buffer[i]);
        return n;
    }

    void log(const char * func, uint8_t logLevel, const char * message)
    {
      if (logLevel >= this->getLogLevel())
      {
        this->printf("[%s] %s", func, message);
      }
    }
    
  #ifdef __USE_TIME__

    char * getDateTimeString() {
      static char buff[] = "YYYY-MM-DD hh:mm:ss\0";
      time_t __now;   // this is the epoch
      tm __tm;        // the structure tm holds time information in a more convient way
      
      time(&__now);               // read the current time
      localtime_r(&__now, &__tm); // update the structure tm with the current time
    
      __tm.tm_year += 1900;
      buff[3] = __tm.tm_year % 10 + '0'; __tm.tm_year /= 10;
      buff[2] = __tm.tm_year % 10 + '0'; __tm.tm_year /= 10;
      buff[1] = __tm.tm_year % 10 + '0'; __tm.tm_year /= 10;
      buff[0] = (uint8_t)__tm.tm_year + '0';
      
      __tm.tm_mon += 1;   // Januar == 0 !!
      buff[6] = __tm.tm_mon % 10 + '0'; __tm.tm_mon /= 10;
      buff[5] = (uint8_t)__tm.tm_mon + '0';
      
      buff[9] = __tm.tm_mday % 10 + '0'; __tm.tm_mday /= 10;
      buff[8] = (uint8_t)__tm.tm_mday + '0';
    
      buff[12] = __tm.tm_hour % 10 + '0'; __tm.tm_hour /= 10;
      buff[11] = (uint8_t)__tm.tm_hour + '0';
    
      buff[15] = __tm.tm_min % 10 + '0'; __tm.tm_min /= 10;
      buff[14] = (uint8_t)__tm.tm_min + '0';
    
      buff[18] = __tm.tm_sec % 10 + '0'; __tm.tm_sec /= 10;
      buff[17] = (uint8_t)__tm.tm_sec + '0';
    
      // return (char*)&buff;
      return &buff[0];
    }    
    char * getTimeString() {
      static char buff[] = "hh:mm:ss\0";
      time_t __now;   // this is the epoch
      tm __tm;        // the structure tm holds time information in a more convient way
      
      time(&__now);               // read the current time
      localtime_r(&__now, &__tm); // update the structure tm with the current time
    
      buff[1] = __tm.tm_hour % 10 + '0'; __tm.tm_hour /= 10;
      buff[0] = (uint8_t)__tm.tm_hour + '0';
    
      buff[4] = __tm.tm_min % 10 + '0'; __tm.tm_min /= 10;
      buff[3] = (uint8_t)__tm.tm_min + '0';
    
      buff[7] = __tm.tm_sec % 10 + '0'; __tm.tm_sec /= 10;
      buff[6] = (uint8_t)__tm.tm_sec + '0';
    
      // return (char*)&buff;
      return &buff[0];
    }
  #endif
    
    using Print::write; // Import other write() methods to support things like write(0) properly
    using Print::printf;
} static StaticLogger;
