#include "ClubRfid.h"

ClubRfid::ClubRfid() {}
ClubRfid::~ClubRfid() {}

void ClubRfid::setup(int ss_pin, int rst_pin) {
  _mfrc522 = new MFRC522(ss_pin, rst_pin);
  SPI.begin(); // SPI-Bus initialisieren
  _mfrc522->PCD_Init(); // MFRC522 initialisieren
  delay(10); //Kurze Pause nach dem Initialisieren 
  _mfrc522->PCD_DumpVersionToSerial(); // Details vom MFRC522 RFID READER / WRITER ausgeben
}

bool ClubRfid::cardPresent() {
  return _mfrc522->PICC_IsNewCardPresent();
}

String ClubRfid::getUid() {
  _mfrc522->PICC_ReadCardSerial();
  
  char idBuffer[32];
  for (unsigned int i = 0; i < _mfrc522->uid.size; i++) {
    byte nib1 = (_mfrc522->uid.uidByte[i] >> 4) & 0x0F;
    byte nib2 = (_mfrc522->uid.uidByte[i] >> 0) & 0x0F;
    idBuffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    idBuffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  idBuffer[_mfrc522->uid.size*2] = '\0';
  return (String)idBuffer;
}
