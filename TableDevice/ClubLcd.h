#ifndef ClubLcd_h
#define ClubLcd_h

#include <Arduino.h>
#include <String.h>
#include <LiquidCrystal_I2C.h>

class ClubLcd {
  private:
    LiquidCrystal_I2C *_lcd;
    bool               _displayBacklightOn = false;
    unsigned long      _lastDisplayOn;
    unsigned int       _displayOnThreshold;
    String             _lcdRow1="";
    String             _lcdRow2="";
    
  public:
    ClubLcd();
    ~ClubLcd();
    void setup();
    void setup(unsigned int threshold);
    void setDisplayBacklightThreshold(unsigned int threshold);
    void displayBacklightOff();
    void printMsg(String msg);
};

#endif
