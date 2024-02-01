#include <Arduino.h>

#include <WiFiMulti.h>


#include <HTTPClient.h>

#include <Arduino_JSON.h>

#include <MD_Parola.h>
#include <MD_MAX72xx.h>

#include <EEPROM.h>

//Own Library's
#include "http.h"
#include "ITAO_EEPROM.h"


// Define Ledmatrix 
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4  // 4 blocks
#define CS_PIN 21

#undef WITH_ID

// create an instance of the MD_Parola class
MD_Parola ledMatrix = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

WiFiMulti wifiMulti;

// Eigenes Struct für die EEPROM Daten;
ITAO_LAUFSCHRIFT_DATEN EEDaten;

//Globale Variablen
const char* ssid = EEDaten.EEssid;
const char* pw = EEDaten.EEIPassword;
const char* uName = EEDaten.EEusername;
const char* uPw = EEDaten.EEpassword;
int* ID = &EEDaten.EEphraseId;
char displayBuffer[HTTP_MAX_LEN + 1] = { '\0' };
http* myHttp;
ITAO_EEPROM* MyEEPROM;

void setup() {
  Serial.begin(19200);

  MyEEPROM = new ITAO_EEPROM();
  InitEEPROM();
  
  wifiMulti.addAP(ssid, pw);

  if ((wifiMulti.run() == WL_CONNECTED)) {
    Serial.println("Connected");
  }

  ledMatrix.begin();           // initialize the object
  ledMatrix.setIntensity(15);  // set the brightness of the LED matrix display (from 0 to 15)
  ledMatrix.displayClear();    // clear led matrix display
  ledMatrix.displayScroll(displayBuffer, PA_CENTER, PA_SCROLL_LEFT, 50);

  myHttp = new http(uName, uPw);

  zeigeDaten();
}

void zeigeDaten() {
  Serial.println("ITAO-Daten");
  Serial.print(" - Username    : ");
  Serial.println(uName);
  Serial.print(" - Password    : ");
  Serial.println(uPw);
  Serial.print(" - ssid: ");
  Serial.println(ssid);
  Serial.print(" - IPassword: ");
  Serial.println(pw);
}

void InitEEPROM() {
  EEDaten = MyEEPROM->ReadEEPROM();
}

void loop() {
  animate();
}

unsigned long lastRequest = 0;
void animate() {
  if (ledMatrix.displayAnimate()) {
    if ((millis() - lastRequest) > 2000) {
      char* displayText = myHttp->GetPhrase();
      fillTextBuffer(displayText);
      ledMatrix.displayReset();
      lastRequest = millis();
    }
    else if(millis() < lastRequest) {
      lastRequest = 0;
    }    
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
        displayBuffer[index++] = sonderzeichen195(*neuerText);
        break;
      default:
        displayBuffer[index++] = *(neuerText);
        break;
    }
    neuerText++;
  }
  displayBuffer[index] = '\0';
  //strncpy(displayBuffer, displayText, HTTP_MAX_LEN);
}
#ifdef WITH_ID
int countDigits(int number) {
  int count = 0;

  // Handle the case where the number is zero separately
  if (number == 0) {
    return 1;
  }

  // Keep dividing the number by 10 until it becomes zero
  while (number != 0) {
    number /= 10;
    ++count;
  }

  return count;
}

void removeCharsFromEnd(char* arr, int countToRemove) {
  if (!arr || countToRemove <= 0) {
    return;  // No action needed if the array is null or countToRemove is not positive
  }

  int len = strlen(arr);

  // Check if countToRemove is greater than or equal to the length
  if (countToRemove >= len) {
    arr[0] = '\0';  // Set the whole array to an empty string
    return;
  }

  // Move the pointer to the new end of the array
  arr += (len - countToRemove);

  *arr = '\0';  // Set the new end of the array
}
#endif