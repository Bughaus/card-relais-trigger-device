
#include "ClubApi.h"

ClubApi::ClubApi() {
}

ClubApi::~ClubApi() {
}

ClubApi::ClubApi(String baseUrl) {
  _baseUrl = baseUrl;
}

ClubApi::ClubApi(String baseUrl, String userAgent) {
  _baseUrl = baseUrl;
  _userAgent = userAgent;
}

void ClubApi::setUserAgent(String sUserAgent) {
  _userAgent = sUserAgent;
}

bool ClubApi::_makeRequest(String methodType, String requestMethod, String postBlock) {
  bool bOk = true;
  _lastStatusCode = -9999;
  
  String sServerPath = _baseUrl + requestMethod;
  _http.begin(_client, sServerPath.c_str());
  
  if (_userAgent.length() > 0)  _http.setUserAgent(_userAgent);
  
  _http.addHeader("Content-Type", "application/json");
  if (_jwt != "") _http.addHeader("authorization", _jwt);
  
  if (methodType == "GET") _lastStatusCode = _http.GET();
  else if (methodType == "POST") _lastStatusCode = _http.POST(postBlock);

  if (_lastStatusCode > 0) {
    _lastResponse = _http.getString();
    if (_lastResponse.length() > 0) {
      DeserializationError error = deserializeJson(_jsondoc, _http.getString());
      if (error) {
        _lastError = (String)"JSONError->" + error.f_str();
        bOk = false;
      }
    }
  } else {
    _lastError = _http.errorToString(_lastStatusCode);
    bOk = false;
  }

  _http.end();
  return bOk;
}

String ClubApi::getError() {
  return _lastError;
}

int ClubApi::getStatusCode() {
  return _lastStatusCode;
}

String ClubApi::getResponse() {
  return _lastResponse;
}

String ClubApi::getResponseAttribute(String attr) {
  return (String)_jsondoc[attr];
}

int ClubApi::getResponseAttributeArrLen(String attr) {
  return _jsondoc[attr].size();
}

String ClubApi::getResponseAttributeArrEl(String attr, int idx) {
  return _jsondoc[attr][idx];
}

bool ClubApi::checkConnection() {
  if (_makeRequest("GET", "/info") && _lastStatusCode == 200 && _lastResponse == "{\"club-api-version\":\"0.1.0\"}") {
    return true;
  } 
  return false;
}

bool ClubApi::loginOk() {
  if (_makeRequest("GET", "/connectionOk") && _lastStatusCode == 200 && _lastResponse == "{\"$ok\":true}") {
    return true;
  } 
  return false;
}

bool ClubApi::authenticate(String username, String password) {
  String credentials = "{\"username\":\"" + username + "\",\"password\":\""+ password +"\"}";
  
  if (_makeRequest("POST", "/users/authenticate", credentials) && _lastStatusCode == 200) {
    _jwt = "Bearer " + (String)_jsondoc["token"];
    return true;
  } 
  return false;
}

bool ClubApi::sessionStart(String cardId) {  
  if (_makeRequest("GET", "/session/start/"+cardId) && _lastStatusCode == 201) {
    return true;
  } 
  return false;
}

bool ClubApi::sessionStop(String cardId) {  
  if (_makeRequest("GET", "/session/stop/"+cardId) && _lastStatusCode == 201) {
    return true;
  } 
  return false;
}

bool ClubApi::sessionLoadActiveCards() {
  if (_makeRequest("GET", "/session/activeCards") && _lastStatusCode == 200) {
    return true;
  } 
  return false;
}
