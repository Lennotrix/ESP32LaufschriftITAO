#ifndef ITAO_EEPROM_H
#define ITAO_EEPROM_H

#include <EEPROM.h>
#include "globals.h"

#define EE_START 0

struct ITAO_LAUFSCHRIFT_DATEN {
  char api_username[30];
  char api_password[30];
  char router_ssid[70];
  char router_password[50];
  char api_endpoint[255];
};

class ITAO_EEPROM {
public:
ITAO_EEPROM();
~ITAO_EEPROM();

  template <typename T>
  static bool WriteEEPROM(const T& data);

  template <typename T>
  static T* ReadEEPROM();

private:
ITAO_LAUFSCHRIFT_DATEN* readEEPROM;
};

template <typename T>
bool ITAO_EEPROM::WriteEEPROM(const T& data) {
  if (!EEPROM.begin(sizeof(T))) {
    Serial.println("EEPROM-ERROR!");
    return false;
  }
  EEPROM.put(EE_START, data);
  delay(500);
  EEPROM.commit();
  return true;
}

template <typename T>
T* ITAO_EEPROM::ReadEEPROM() {
  if (!EEPROM.begin(sizeof(T))) {
    Serial.println("EEPROM ERROR!");
    ESP.restart();
  }
  T* readData = new T;
  EEPROM.get(EE_START, *readData);
  delay(500);
  Serial.println("Read data from EEPROM!");
  return readData;
}
#endif