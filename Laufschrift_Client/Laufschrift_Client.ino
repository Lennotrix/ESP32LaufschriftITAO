#include <Arduino.h>

#include <WiFiMulti.h>

#include <HTTPClient.h>

#include <Arduino_JSON.h>

#include <MD_Parola.h>
#include <MD_MAX72xx.h>

#include <EEPROM.h>
#include <UrlEncode.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Own Library's
#include "http.h"
#include "ITAO_EEPROM.h"

// Define Ledmatrix
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4 // 4 blocks
#define CS_PIN 21

// Status Led Pins
#define ERROR_PIN 27
#define BOOT_PIN 26
#define RUNNING_PIN 25

#undef WITH_ID

TaskHandle_t Task_Booting_Handle;

// create an instance of the MD_Parola class
MD_Parola ledMatrix = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

WiFiMulti wifiMulti;

// Eigenes Struct für die EEPROM Daten;
ITAO_LAUFSCHRIFT_DATEN EEDaten;

// Globale Variablen
const char *ssid = EEDaten.EEssid;
const char *pw = EEDaten.EEIPassword;
const char *uName = EEDaten.EEusername;
const char *uPw = EEDaten.EEpassword;
char displayBuffer[HTTP_MAX_LEN + 1] = {'\0'};
http *myHttp;
ITAO_EEPROM *MyEEPROM;
String MobileNumber = "";
String APIKey = "";
bool isBooting = true;

void setup()
{
  PinSetup();
  xTaskCreate(Booting, "BootingTask", 10000, NULL, 1, &Task_Booting_Handle);

  Serial.begin(19200);

  MyEEPROM = new ITAO_EEPROM();
  InitEEPROM();

  wifiMulti.addAP(ssid, pw);
  wifiMulti.addAP("itao", "kleinholz");

  if ((wifiMulti.run() == WL_CONNECTED))
  {
    Serial.println("Connected");
  }
  else
  {
    ErrorLed(true);
  }

  ledMatrix.begin();          // initialize the object
  ledMatrix.setIntensity(15); // set the brightness of the LED matrix display (from 0 to 15)
  ledMatrix.displayClear();   // clear led matrix display
  ledMatrix.displayScroll(displayBuffer, PA_CENTER, PA_SCROLL_LEFT, 50);

  myHttp = new http(uName, uPw);

  RunLed(true);
  BootLed(false);
  isBooting = false;
  vTaskDelete(Task_Booting_Handle);
}

void Booting(void *parameter)
{
  while (isBooting)
  {
    BootLed(true);
    delay(500);
    BootLed(false);
    delay(500);
  }
  BootLed(false);
}

void PinSetup()
{
  pinMode(ERROR_PIN, OUTPUT);
  pinMode(BOOT_PIN, OUTPUT);
  pinMode(RUNNING_PIN, OUTPUT);

  digitalWrite(ERROR_PIN, HIGH);
  digitalWrite(BOOT_PIN, HIGH);
  digitalWrite(RUNNING_PIN, HIGH);

  delay(1000);

  digitalWrite(ERROR_PIN, LOW);
  digitalWrite(BOOT_PIN, LOW);
  digitalWrite(RUNNING_PIN, LOW);

  digitalWrite(BOOT_PIN, HIGH);
}

void ErrorLed(bool val)
{
  digitalWrite(ERROR_PIN, val);
}
void BootLed(bool val)
{
  digitalWrite(BOOT_PIN, val);
}
void RunLed(bool val)
{
  digitalWrite(RUNNING_PIN, val);
}

void zeigeDaten()
{
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

void InitEEPROM()
{
  EEDaten = MyEEPROM->ReadEEPROM();
}

void loop()
{
  animate();
  if (Serial.available())
  {
    String sInput = Serial.readStringUntil(';');
    if (sInput.substring(0, 1) == (const char *)'#')
    {
      if (sInput.substring(1) == (const char *)"Restart;")
      {
        ESP.restart();
      }
      if (sInput.substring(1, sInput.indexOf('%')) == (const char *)"EEPROM")
      {
        JSONVar eepromJson = JSON.parse(sInput.substring(sInput.indexOf('%'), sInput.indexOf(';') - 1));
        if (JSON.typeof(jsonObject) == "undefined")
        {

        }
        else{
          ErrorLed(true);
        }
      }
    }
  }
}

unsigned long lastRequest = 0;
void animate()
{
  if (ledMatrix.displayAnimate())
  {
    if ((millis() - lastRequest) > 2000)
    {
      char *displayText = myHttp->GetPhrase();
      fillTextBuffer(displayText);
      Serial.println(displayText);
      ledMatrix.displayShutdown(true);
      ledMatrix.begin();          // initialize the object
      ledMatrix.setIntensity(15); // set the brightness of the LED matrix display (from 0 to 15)
      ledMatrix.displayClear();   // clear led matrix display
      ledMatrix.displayScroll(displayBuffer, PA_CENTER, PA_SCROLL_LEFT, 50);
      lastRequest = millis();
    }
    else if (millis() < lastRequest)
    {
      lastRequest = 0;
    }
  }
}

char sonderzeichen195(char code)
{
  char ergebnis = ' ';
  switch (code)
  {
  case 188: // ü
    ergebnis = 252;
    break;
  case 164: // ä
    ergebnis = 228;
    break;
  case 182: // ö
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

void fillTextBuffer(char *neuerText)
{
  int index = 0;
  while ((*neuerText != '\0') && (index < HTTP_MAX_LEN - 1))
  {
    switch (*neuerText)
    {
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
  // strncpy(displayBuffer, displayText, HTTP_MAX_LEN);
}
#ifdef WITH_ID
int countDigits(int number)
{
  int count = 0;

  // Handle the case where the number is zero separately
  if (number == 0)
  {
    return 1;
  }

  // Keep dividing the number by 10 until it becomes zero
  while (number != 0)
  {
    number /= 10;
    ++count;
  }

  return count;
}

void removeCharsFromEnd(char *arr, int countToRemove)
{
  if (!arr || countToRemove <= 0)
  {
    return; // No action needed if the array is null or countToRemove is not positive
  }

  int len = strlen(arr);

  // Check if countToRemove is greater than or equal to the length
  if (countToRemove >= len)
  {
    arr[0] = '\0'; // Set the whole array to an empty string
    return;
  }

  // Move the pointer to the new end of the array
  arr += (len - countToRemove);

  *arr = '\0'; // Set the new end of the array
}
#endif

void sendWhatsApp(String message)
{
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + MobileNumber + "&apikey=" + APIKey + "&text=" + urlEncode(message);
  HTTPClient http;
  http.begin(url);

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200)
  {
    Serial.print("Message sent successfully");
  }
  else
  {
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}