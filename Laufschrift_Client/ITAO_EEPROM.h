#include <EEPROM.h>

#define EE_START 0

struct ITAO_LAUFSCHRIFT_DATEN {
  char EEusername[30];
  char EEpassword[30];
  char EEssid[70];
  char EEIPassword[50];
};

class ITAO_EEPROM {
public:

  bool WriteEEPROM(ITAO_LAUFSCHRIFT_DATEN werte);

  ITAO_LAUFSCHRIFT_DATEN ReadEEPROM();

private:
};
