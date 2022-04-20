#include "ClubLcd.h"

ClubLcd::ClubLcd() {}
ClubLcd::~ClubLcd() {}

void ClubLcd::printMsg(String msg) {
  if      (_lcdRow1 == "") _lcdRow1 = msg;
  else if (_lcdRow2 == "") _lcdRow2 = msg;
  else {
    _lcdRow1 = _lcdRow2;
    _lcdRow2 = msg;
  }
  
  _lcd->clear();
  _lcd->setCursor(0,0);
  _lcd->print(_lcdRow1);
  _lcd->setCursor(0,1);
  _lcd->print(_lcdRow2);
  displayBacklightOn();
}

void ClubLcd::setup() {
  _lcd = new LiquidCrystal_I2C(0x27,16,2);
  _lcd->init();
  _lcd->setCursor(0,0);
}

void ClubLcd::setup(unsigned int threshold) {
  setDisplayBacklightThreshold(threshold);
  setup();
}

void ClubLcd::setDisplayBacklightThreshold(unsigned int threshold) {
  _displayOnThreshold = threshold;
}

void ClubLcd::displayBacklightOff() {
  if (_displayBacklightOn && millis() - _lastDisplayOn > _displayOnThreshold) {
    _lcd->noBacklight();
    _displayBacklightOn = false;
  }
}

void ClubLcd::displayBacklightOn() {
  _lastDisplayOn = millis();
  _displayBacklightOn = true;
  _lcd->backlight();
}
