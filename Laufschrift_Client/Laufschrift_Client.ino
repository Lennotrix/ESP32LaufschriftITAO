#include <HTTPClient.h>

#include "WiFi.h"

#include <Arduino_JSON.h>

#include <MD_Parola.h>
#include <MD_MAX72xx.h>

#include <EEPROM.h>
#include <UrlEncode.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Own Library's
#include "http.h"
#include "globals.h"

// Function prototypes
void setup();
void loop();
void zeigeDaten();
void InitEEPROM();
void ESPDayCheck();
void animate();
void SerialInput();
void handleJsonError();
void updateEEPROM(JSONVar eepromJson);
bool validateJson(JSONVar json);
void LedMatrixReset();
char sonderzeichen195(char code);
void fillTextBuffer(char *neuerText);
void sendWhatsApp(String message);
void Booting(void *parameter);
void PinSetup();
void ErrorLed(bool val);
void BootLed(bool val);
void RunLed(bool val);

// Define Ledmatrix
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define CS_PIN 21
#define CLK_PIN   18
#define DATA_PIN  23

// Extra Thread for Blinking Boot Led
TaskHandle_t Task_Booting_Handle;

// create an instance of the MD_Parola class
//MD_Parola ledMatrix = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_Parola ledMatrix = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// WiFiMulti wifiMulti;

// Eigenes Struct für die EEPROM Daten;
ITAO_LAUFSCHRIFT_DATEN EEData;

WiFiClass wifi;

// Class Insider
#define PauseJetzt delay

#define wifiFlushCooldown 3600000

// Globale Variablen
const char *ssid = EEData.EEssid;
const char *pw = EEData.EEIPassword;
const char *uName = EEData.EEusername;
const char *uPw = EEData.EEpassword;
const char *endpointUrl = EEData.EEendpoint;
char displayBuffer[HTTP_MAX_LEN + 1] = {'\0'};
String MobileNumber = "";
String APIKey = "";
bool isBooting = true;
unsigned long lastRequest = 0;
http *myHttp;
ITAO_EEPROM *MyEEPROM;
int wifiStatus;
long lastHour = 1;

void setup()
{
  Serial.begin(19200);

  #ifdef NO_WIFI
  Serial.println("No Wifi Mode!");
  #endif
  #ifdef DEBUG
  Serial.println("Debug Mode!");
  #endif
  SPI.setDataMode(SPI_MODE3);
  PinSetup();
  xTaskCreate(Booting, "BootingTask", 10000, NULL, 15, &Task_Booting_Handle);

  MyEEPROM = new ITAO_EEPROM();
  InitEEPROM();

  #ifndef NO_WIFI
  wifi.begin(ssid, pw);
  while (wifi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to open SSID: ");
    Serial.println(ssid);
    Serial.print("Password:");
    Serial.println(pw);
    Serial.println(wifi.status());
    SerialInput();
    PauseJetzt(5000);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(wifi.localIP());
#endif

  ledMatrix.begin();          // initialize the object
  ledMatrix.setIntensity(MATRIX_INTENSITY); // set the brightness of the LED matrix display (from 0 to 15)
  ledMatrix.displayClear();   // clear led matrix display
  ledMatrix.displayScroll(displayBuffer, PA_CENTER, PA_SCROLL_LEFT, MATRIX_PACE);

  myHttp = new http(uName, uPw, &EEData);

  zeigeDaten();

  RunLed(true);
  BootLed(false);
  isBooting = false;
  vTaskDelete(Task_Booting_Handle);
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
  Serial.print(" - Endpoint: ");
  Serial.println(endpointUrl);
}

void InitEEPROM()
{
  EEData = MyEEPROM->ReadEEPROM();
}

void loop()
{
  animate();
  if (Serial.available())
  {
    SerialInput();
  }
  ESPDayCheck();
}
void ESPDayCheck(){
  //int timeNow = millis();
  int hoursRunning = millis() / 3600000;
  //int minutesRunning = millis() / 60000;
  //int hoursNow = (timeNow % 86400000) / 3600000;
  //int minutes = ((timeNow % 86400000) % 3600000) / 60000;
  if(hoursRunning >= 2){
    Serial.println("Reboot");
    ESP.restart();
  }
}
void animate()
{
  if (ledMatrix.displayAnimate())
  {
    if ((millis() - lastRequest) > 2000)
    {
      char *displayText = myHttp->GetPhrase();
      fillTextBuffer(displayText);
      Serial.println(displayText);
      lastRequest = millis();
      LedMatrixReset();
    }
    else if (millis() < lastRequest)
    {
      lastRequest = 0;
    }
  }
}
void SerialInput() {
    String sInput = Serial.readStringUntil(';');

    if (sInput.startsWith("#")) {
        String command = sInput.substring(1, sInput.indexOf('%'));

        if (command == "Restart") {
            ESP.restart();
        } else if (command == "EEPROM") {
            String jsonString = sInput.substring(sInput.indexOf('{'), sInput.lastIndexOf('}') + 1);
            JSONVar eepromJson = JSON.parse(jsonString);

            if (JSON.typeof(eepromJson) == "undefined") {
                handleJsonError();
            } else {
                updateEEPROM(eepromJson);
            }
        }
    }
}

void handleJsonError() {
    ErrorLed(true);
    PauseJetzt(2000);
    ErrorLed(false);
}

void updateEEPROM(JSONVar eepromJson) {
    if (validateJson(eepromJson)) {
        ITAO_LAUFSCHRIFT_DATEN newITAOEEPROM;

        strncpy(newITAOEEPROM.EEusername, eepromJson["EEusername"], 30);
        strncpy(newITAOEEPROM.EEpassword, eepromJson["EEpassword"], 30);
        strncpy(newITAOEEPROM.EEssid, eepromJson["EEssid"], 70);
        strncpy(newITAOEEPROM.EEIPassword, eepromJson["EEIPassword"], 50);
        strncpy(newITAOEEPROM.EEendpoint, eepromJson["EEEndpoint"], 255);

        if (MyEEPROM->WriteEEPROM(newITAOEEPROM)) {
            ESP.restart();
        }
    }
}

bool validateJson(JSONVar json) {
    return json.hasOwnProperty("EEusername") &&
           json.hasOwnProperty("EEpassword") &&
           json.hasOwnProperty("EEssid") &&
           json.hasOwnProperty("EEIPassword") &&
           json.hasOwnProperty("EEEndpoint");
}


void LedMatrixReset()
{
  ledMatrix.displayShutdown(true);
  ledMatrix.begin();          // initialize the object
  ledMatrix.setIntensity(15); // set the brightness of the LED matrix display (from 0 to 15)
  ledMatrix.displayClear();   // clear led matrix display
  ledMatrix.displayScroll(displayBuffer, PA_CENTER, PA_SCROLL_LEFT, MATRIX_PACE);
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

void Booting(void *parameter)
{
  while (isBooting)
  {
    BootLed(true);
    PauseJetzt(500);
    BootLed(false);
    PauseJetzt(500);
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

  PauseJetzt(1000);

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
