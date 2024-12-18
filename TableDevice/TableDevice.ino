

#include "ClubRfid.h"
#include "ClubLcd.h"
#include "ClubNet.h"
#include "ClubApi.h"
#include "CardList.h"

/* we get the secrets from a external header that will be generadted by the build script */
#include "secret_credentials.h"

/* if OTA password is given? */
#ifdef OTA_FLASH_PASSWORD
  #include <ArduinoOTA.h>
#endif

/* ************************************************************
 * CONFIGURATIONS
 * ************************************************************ */
#define SS_PIN          15         // Pin: D8
#define RST_PIN          0         // Pin: D3
#define LINE_TRIGGER_PIN 2         // Pin: D4
#define RELAIS_COM_PIN  16
#define DISPLAY_TIMEOUT_THRESHOLD 10000

/* ************************************************************
 * GLOBAL VARIABLES
 * ************************************************************ */
bool         INIT_OK=true;
int          LAST_CARD_CNT=0;
bool         TRIGGER_CARD_CHANGE=false;
String       init_error="";
ClubLcd      lcd;
ClubRfid     rfid;
ClubApi      api(SECRET_API_BASEURL, SECRET_API_USERAGENT);
ClubNet      net;
CardList     cardList(4);

/* ************************************************************************************************************************
 * FUNCTIONS
 * ************************************************************************************************************************ */
void log(String msg, ClubLcd *lcd=0, int row=-1, int col=-1) {
  Serial.println(msg);
  if (!lcd) return;
  lcd->printMsg(msg, row, col);
}

void log_api_error(ClubApi *api) {
  log((String)"HTTP status code: " + (String)api->getStatusCode());
  log((String)"Response: " + api->getResponse());
  log((String)"Error: " + api->getError());
}

String calculateSessionInterval(String sStart, String sEnd) {
  sStart = sStart.substring(0, sStart.length()-3);
  sEnd = sEnd.substring(0, sEnd.length()-3);
  int iStart = sStart.toInt();
  int iEnd = sEnd.toInt() ;
  int diffH = ((iEnd - iStart) / 60) / 60;
  int diffM = ((iEnd - iStart) / 60) % 60;

  String output =   (String)((diffH < 10) ? "0" : "") + (String)diffH + (String)":"
                  + (String)((diffM < 10) ? "0" : "") + (String)diffM;
  return output;
}

/* ************************************************************************************************************************
 * SETUP
 * ************************************************************************************************************************ */
void setup() {
  
  /* ************************************************************
   * Serial and Pinouts 
   * ************************************************************ */
  Serial.begin(115200);
  pinMode(RELAIS_COM_PIN, OUTPUT);
  pinMode(LINE_TRIGGER_PIN, INPUT);

  /* ************************************************************
   * Declare vars that collide with the jump labels
   * ************************************************************ */
  int iActiveSessions=0;
  int i=0;
  
  /* ************************************************************
   * LCD INIT 
   * ************************************************************ */
  lcd.setup(DISPLAY_TIMEOUT_THRESHOLD);
  log("Starting up...", &lcd);
  
  /* ************************************************************
   * RFID INIT
   * ************************************************************ */
  rfid.setup(SS_PIN, RST_PIN);
  log("RFID ready!",  &lcd);

  /* ************************************************************
   * SETUP OTA IF OTA PW IS DEFINED
   * ************************************************************ */
#ifdef OTA_FLASH_PASSWORD
    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPassword(OTA_FLASH_PASSWORD);
    ArduinoOTA.onStart([]() {
      log("OTA flashing", &lcd);
      log("", &lcd);
      log("[", &lcd, 1, 0);
      log("]", &lcd, 1, 15);
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      String progressOut = "";
      int progCnt = progress / (total / 14);
      for (int i = 0; i < progCnt; i++) { progressOut += "#"; }
      log(progressOut, &lcd, 1, 1);
    });
    ArduinoOTA.onError([](ota_error_t error) {
      log("OTA flash error:");
      if (error == OTA_AUTH_ERROR) log("Auth Failed", &lcd);
      else if (error == OTA_BEGIN_ERROR) log("Begin Failed", &lcd);
      else if (error == OTA_CONNECT_ERROR) log("Connect Failed", &lcd);
      else if (error == OTA_RECEIVE_ERROR) log("Receive Failed", &lcd);
      else if (error == OTA_END_ERROR) log("End Failed", &lcd);
      delay(5000);
    });
    ArduinoOTA.onEnd([]() {
      log("OTA flash", &lcd);
      log("successful!", &lcd);
      delay(5000);
    });
    ArduinoOTA.begin();
    log("OTA ready!",  &lcd);
#endif

  /* ************************************************************
   * CONNECT WIFI NETWORK
   * ************************************************************ */
  log("Connect WIFI...", &lcd);
  log((String)"SSID=" + SECRET_WIFI_SSID);
  if (!net.setupWifi(SECRET_WIFI_SSID, SECRET_WIFI_KEY)) {
    init_error=net.getLastError();
    goto INIT_FAILED;
  }
  log("IP=" + WiFi.localIP().toString());
  log("WIFI ready!", &lcd);

  /* ************************************************************
   * API CHECK
   * ************************************************************ */
  log("Check API...", &lcd);
  if (!api.checkConnection()) {
    init_error="No API connect!";
    log_api_error(&api);
    goto INIT_FAILED;
  }
  log("API OK.", &lcd);
  
  /* ************************************************************
   * API LOGIN
   * ************************************************************ */
  log("Login to DB...", &lcd);
  if(!api.authenticate(SECRET_TABLE_USERNAME, SECRET_TABLE_PASSWORD)) {
    init_error="No DB Login!";
    log_api_error(&api);
    goto INIT_FAILED;
  }
  log("DB Login OK.", &lcd);

  /* ************************************************************
   * CHECK IF THERE ARE UNCLOSED SESSION LEFT
   * ************************************************************ */
  log("Active sessions?", &lcd);
  if(!api.sessionLoadActiveCards()) {
    init_error="No DB Login!";
    log_api_error(&api);
    goto INIT_FAILED;
  }
  iActiveSessions = api.getResponseAttributeArrLen("cardIds");
  log((String)"Cnt=" + (String)iActiveSessions);

  if(iActiveSessions > 0) {
    for (i = 0; i < iActiveSessions; i ++) {
      cardList.add(api.getResponseAttributeArrEl("cardIds", i));
      log("Restored CardId " + api.getResponseAttributeArrEl("cardIds", i));
    }
    log("Restored active sessions.");
  }
  else log("No active Sessions found.");

  goto INIT_OK;

INIT_FAILED:
  log("Init failed!!!", &lcd);
  if (init_error.length() > 0) {
    log(init_error, &lcd);
  }
  INIT_OK=false;
  goto INIT_END;
  
INIT_OK:
  log("Init OK!");
  TRIGGER_CARD_CHANGE=true;
  
INIT_END:
  log("Init finished.");
}


/* ************************************************************************************************************************
 * LOOP
 * ************************************************************************************************************************ */
void loop() {
  // do we have a OTA passphrase? Then enable STA
#ifdef OTA_FLASH_PASSWORD
  ArduinoOTA.handle();
#endif

  if (digitalRead(LINE_TRIGGER_PIN) == LOW) {
    lcd.displayBacklightOn();
  }

  /*
   * Only when INIT was succesful and a card is presented
   */
  if (INIT_OK && rfid.cardPresent()) {
    /* ************************************************************
     * Get UID
     * ************************************************************ */
    String cardId=rfid.getUid();
    if (cardId.length() == 0) {
      goto END_OF_LOOP;
    }
    log((String)"Card UId=" + cardId);  

    /* ************************************************************
     * API Connection alive?
     * ************************************************************ */
    if (!api.loginOk()) {
      log("Relogin to API");
      if(!api.authenticate(SECRET_TABLE_USERNAME, SECRET_TABLE_PASSWORD)) {
        log_api_error(&api);
        goto REQUEST_ERROR;
      }
      log("Relogin OK.", &lcd);
    } 

    /* ************************************************************
     * Is the card present in the card list?
     * 1. yes - stop current session
     * 2. no  - start new session
     *     -> check if card can be added (free space in card list)
     * ************************************************************ */
    
    /* ******* START SESSION ******** */
    if (!cardList.isInList(cardId)) {

      if (!cardList.hasSpace()) {
        log("Zu viele", &lcd);
        log("Spieler!", &lcd);
      } else {
        if (!api.sessionStart(cardId)) {
          /* if an error occures, the card is possible used at another machine */
          if (api.getStatusCode() == 400 && api.getResponseAttribute("message") == "There is already an active session! Stop this one first before starting a new one!") {
            log("Du spielst schon", &lcd);
            log(api.getResponseAttribute("table"), &lcd);
            TRIGGER_CARD_CHANGE=true;
          }
          else {
            log_api_error(&api);
            delay(3000);
            goto REQUEST_ERROR;
          }
        }
        else {
          log(api.getResponseAttribute("member"), &lcd);
          log((String)"Start " + api.getResponseAttribute("time"), &lcd);
          cardList.add(cardId);
        }
      }
    } 
    
    /* ******* STOP SESSION ******** */
    else {  
      if (!api.sessionStop(cardId)) {
        log_api_error(&api);
        goto REQUEST_ERROR;
      }

      String interval = calculateSessionInterval(api.getResponseAttribute("startedAt"), api.getResponseAttribute("endedAt"));
      log(api.getResponseAttribute("member"), &lcd);
      log((String)"Dauer " + interval, &lcd);
      cardList.del(cardId);
    }

    
  }

goto END_OF_LOOP;

REQUEST_ERROR:
  log("A request error occured!");
  
END_OF_LOOP:
  /* ******* Did the number of cards change? Then info in LCD and change Relais state. ******** */
  if (TRIGGER_CARD_CHANGE || LAST_CARD_CNT != cardList.cnt()) {
    LAST_CARD_CNT = cardList.cnt();

    /* ******* Switch Relais ******** */
    if (LAST_CARD_CNT > 0)  digitalWrite(RELAIS_COM_PIN, HIGH);
    else                    digitalWrite(RELAIS_COM_PIN, LOW);

    delay(3000);
    log((LAST_CARD_CNT == 0 ? (String)"Kein " : (String)LAST_CARD_CNT) + (String)" Spieler", &lcd);
    log("angemeldet.", &lcd);
    TRIGGER_CARD_CHANGE=false;
  }
  
  /* ******* Backlight Off Call ******** */
  lcd.displayBacklightOff();
}
