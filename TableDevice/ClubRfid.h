#ifndef ClubRfid_h
#define ClubRfid_h

#include <Arduino.h>
#include <String.h>
#include <SPI.h>
#include <MFRC522.h>

class ClubRfid {
  private:
    MFRC522 *_mfrc522;
    
  public:
    ClubRfid();
    ~ClubRfid();

    void setup(int ss_pin, int rst_pin);
    bool cardPresent();
    String getUid();
};

#endif
