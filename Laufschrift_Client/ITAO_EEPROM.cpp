#include <EEPROM.h>
#include "ITAO_EEPROM.h"
#include "globals.h"

#define EE_START 0

bool ITAO_EEPROM::WriteEEPROM(ITAO_LAUFSCHRIFT_DATEN werte){
  if (!EEPROM.begin(sizeof(ITAO_LAUFSCHRIFT_DATEN))) {
    Serial.println("EEPROM-ERROR!");
    return false;
  }
  EEPROM.put(EE_START, werte);
  delay(500);
  EEPROM.commit();
  return true;
}

ITAO_LAUFSCHRIFT_DATEN ITAO_EEPROM::ReadEEPROM(){
  if (!EEPROM.begin(sizeof(ITAO_LAUFSCHRIFT_DATEN))) {
    Serial.println("EEPROM ERROR!");
    ESP.restart();
  }
  ITAO_LAUFSCHRIFT_DATEN readEEPROM;
  EEPROM.get(EE_START, readEEPROM);
  delay(500);
  Serial.println("Read ITAO data type!");
  return readEEPROM;
}