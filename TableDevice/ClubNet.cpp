#include "ClubNet.h"

ClubNet::ClubNet() {}
ClubNet::~ClubNet() {}

bool ClubNet::setupWifi(String ssid, String key) {
    WiFi.begin(ssid, key);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      if (WiFi.status() == WL_WRONG_PASSWORD) {
        _lastError = "Wrong password!";
        return false;
      }
      if (WiFi.status() == 1) {
        _lastError = "SSID not found!";
        return false;
      }
    }
    return true;
}

String ClubNet::getLastError() {
  return _lastError;
}
