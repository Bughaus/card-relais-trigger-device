#ifndef ClubApi_h
#define ClubApi_h

#include <Arduino.h>
#include <String.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

class ClubApi {
  private:
    String     _baseUrl="";
    String     _jwt;
    HTTPClient _http;
    WiFiClient _client;
    int        _lastStatusCode=-9999;
    String     _lastResponse="";
    String     _lastError="";
    String     _userAgent="";
    StaticJsonDocument<1024> _jsondoc;
    
    bool    _isValidMethod(String requestMethod);
    bool    _makeRequest(String methodType, String requestMethod, String postBlock="");
  public:
    ClubApi();
    ClubApi(String baseUrl);
    ClubApi(String baseUrl, String userAgent);
    ~ClubApi();

    void setUserAgent(String userAgent);
    int getStatusCode();
    String getError();
    String getResponse();
    String getResponseAttribute(String attr);
    int    getResponseAttributeArrLen(String attr);
    String getResponseAttributeArrEl(String attr, int idx);
    bool checkConnection();
    bool authenticate(String username, String password);
    bool sessionStart(String cardId);
    bool sessionStop(String cardId);
    bool sessionLoadActiveCards();
};

#endif
