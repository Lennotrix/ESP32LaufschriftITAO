#include <EEPROM.h>
#include "globals.h"

struct ITAO_LAUFSCHRIFT_DATEN {
  char EEusername[30];
  char EEpassword[30];
  char EEssid[70];
  char EEIPassword[50];
  char EEendpoint[255];
};

class ITAO_EEPROM {
public:

  bool WriteEEPROM(ITAO_LAUFSCHRIFT_DATEN werte);

  ITAO_LAUFSCHRIFT_DATEN ReadEEPROM();

private:
};
