#ifndef _LITTLE_FS_LOGGER_
#define _LITTLE_FS_LOGGER_

#include <arduino.h>
#include <LittleFS.h>

class LittleFSlogger: public Print {
  protected:
    String filename;
    File logfile;
    bool usefile = false;
    bool isOpen = false;
    bool newline = true;
    bool toSerial = true;

  public:
    LittleFSlogger(String fname = "") {}

    bool begin(String fname = "", bool toSer = true) {
    	// if toSer is true, logger will print to Serial as well

      filename = fname;
      usefile = (filename.length() > 0);

			toSerial = toSer;

      if (logfile) logfile.close();
      if (usefile) {
	      logfile = LittleFS.open(filename, "a");
	      isOpen = (bool)logfile;
      	return isOpen;
      }
      return true;
    }

		void flush(void){
			if (usefile && isOpen) logfile.flush();
		  if (toSerial) Serial.flush();
		}

    void close(void) {
      if (logfile) logfile.close();
      isOpen = false;
    }

    void beforeWrite() {
//        Serial.println("[beforeWrite]");
        if (usefile && !isOpen) begin(filename, toSerial);
        if (newline) {
        	if (usefile) logfile.flush();
          newline = false;
          this->print(millis());
          this->print(":");
        }
    }

    size_t write(uint8_t c) override
    {
//        Serial.println("[write(uint8_t c)]");
        beforeWrite();
        newline = (c == '\n');
        if (toSerial) Serial.write(c);
        if (usefile) return logfile.write(c);
        return 1;
    }

    size_t write(const uint8_t *buffer, size_t size) override
    {
//        Serial.print("[write(const uint8_t *buffer, size_t size)]");
//        Serial.print((const char*)buffer);
//        Serial.print(" : ");
//        Serial.println(size);
        size_t n = 0;
        for (size_t i = 0; i < size; i++) n += write((uint8_t)buffer[i]);
        return n;
//        if (!isOpen) begin();
//        return logfile.write((const char*)buffer, size);
    }
    using Print::write; // Import other write() methods to support things like write(0) properly
    using Print::printf;
};
#endif