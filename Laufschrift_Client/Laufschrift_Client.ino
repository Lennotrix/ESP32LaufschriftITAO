#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include "http.h"

#include <HTTPClient.h>

#include <Arduino_JSON.h>

#include <MD_Parola.h>
#include <MD_MAX72xx.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4  // 4 blocks
#define CS_PIN 21

#define DEBUG

// create an instance of the MD_Parola class
MD_Parola ledMatrix = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

WiFiMulti wifiMulti;

const char* ssid = "TSST-SN";
const char* pw = "123456789";
char meinTextPuffer[HTTP_MAX_LEN + 1] = { '\0' };
char* pPhraseURL = "http://10.1.1.30:5047/api/v01/phrase?id=2";
const char* pLoginURL = "http://10.1.1.30:5047/api/v01/Auth/Login";
const char* uName = "test001";
const char* uPw = "test001";
http myHttp(uName, uPw);

void setup() {
  Serial.begin(19200);

  wifiMulti.addAP(ssid, pw);

  if ((wifiMulti.run() == WL_CONNECTED)) {
    Serial.println("Connected");
  }
  ledMatrix.begin();           // initialize the object
  ledMatrix.setIntensity(15);  // set the brightness of the LED matrix display (from 0 to 15)
  ledMatrix.displayClear();    // clear led matrix display
  ledMatrix.displayScroll(meinTextPuffer, PA_CENTER, PA_SCROLL_LEFT, 50);
  //http myHttp(uName, uPw);
}

void loop() {
  animate();
}

void animate() {
  if (ledMatrix.displayAnimate()) {
    char* displayText = myHttp.GetPhrase(pPhraseURL);
    fillTextBuffer(displayText);
    ledMatrix.displayReset();
  }
}

char sonderzeichen195(char code) {
  char ergebnis = ' ';
  switch (code) {
    case 188:  // ü
      ergebnis = 252;
      break;
    case 164:  // ä
      ergebnis = 228;
      break;
    case 182:  // ö
      ergebnis = 246;
      break;
    case 132:
      ergebnis = 196;
      break;
    case 156:
      ergebnis = 220;
      break;
    case 150:
      ergebnis = 214;
      break;
    default:
      Serial.print("Unbekanntes 195-Zeichen --> ");
      Serial.println((int)code);
      break;
  }
  return ergebnis;
}

void fillTextBuffer(char* neuerText) {
  int index = 0;
  while ((*neuerText != '\0') && (index < HTTP_MAX_LEN - 1)) {
    switch (*neuerText) {
      case 195:
        neuerText++;
        meinTextPuffer[index++] = sonderzeichen195(*neuerText);
        break;
      default:
        meinTextPuffer[index++] = *(neuerText);
        break;
    }
    neuerText++;
  }
  meinTextPuffer[index] = '\0';
  //strncpy(meinTextPuffer, displayText, HTTP_MAX_LEN);
}
