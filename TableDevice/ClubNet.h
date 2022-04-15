#ifndef ClubNet_h
#define ClubNet_h

#include <Arduino.h>
#include <String.h>
#include <ESP8266WiFi.h>


class ClubNet {
  private:
    String _lastError = "";

  public:
    ClubNet();
    ~ClubNet();
    bool setupWifi(String ssid, String key);
    String getLastError();
};

#endif
