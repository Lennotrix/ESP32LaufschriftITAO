#include "globals.h"
#include <Arduino_JSON.h>
#include <HTTPClient.h>
#include "ITAO_EEPROM.h"

class http
{
public:
  http(const char *uName, const char *pw, ITAO_LAUFSCHRIFT_DATEN *EEData);
  ~http();
  char *GetPhrase();
  char *Login();

private:
  void appendPhrase(JSONVar &jsonObject, char *phrase);
  const char *uName;
  const char *uPw;
  ITAO_LAUFSCHRIFT_DATEN *EEData;
  char endpoint[ENDPOINT_MAX_LENGTH + 1] = {'\0'};
  char buffer[HTTP_MAX_LEN + 1] = {'\0'};
  char bearerBuffer[HTTP_MAX_LEN + 1] = {'\0'};
  char *pBearer;
};