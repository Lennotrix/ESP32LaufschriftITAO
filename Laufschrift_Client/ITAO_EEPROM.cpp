#include <EEPROM.h>
#include "ITAO_EEPROM.h"
#include "globals.h"

ITAO_EEPROM::ITAO_EEPROM(){
  this->readEEPROM = new ITAO_LAUFSCHRIFT_DATEN;
}
ITAO_EEPROM::~ITAO_EEPROM(){
  delete this->readEEPROM;
}