#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include "http.h"

#include <HTTPClient.h>

#include <Arduino_JSON.h>
#include <assert.h>

#include <MD_Parola.h>
#include <MD_MAX72xx.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4  // 4 blocks
#define CS_PIN 21

// create an instance of the MD_Parola class
MD_Parola ledMatrix = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

WiFiMulti wifiMulti;

const char* ssid = "itao";
const char* pw = "kleinholz";
char meinTextPuffer[HTTP_MAX_LEN + 1] = "Hier könnte Ihr Text stehen!";


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
}

void loop() {
  http myHttp("http://10.1.122.19:7115/api/v01/phrase");

  char* displayText = myHttp.GetPhrase();
  fillTextBuffer(displayText);
  Serial.println(displayText);
  animate();
  //delay(3000);
  //http.end();
}

void animate() {
  if (ledMatrix.displayAnimate()) {
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
