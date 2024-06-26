#include "http.h"
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "globals.h"

// Variablen

#ifdef DEBUG
const char* phraseEndpoint = "/api/v01/phrase?id=";
#endif
#ifndef DEBUG
const char* phraseEndpoint = "/api/v01/phrase/device";
#endif

HTTPClient httpCli;

http::http(ITAO_LAUFSCHRIFT_DATEN *EEData)
{
  this->EEData = EEData;
  this->pBearer = Login();
}

http::~http()
{
}

char *http::GetPhrase()
{
  char *pEndpoint = &endpoint[0];
  *pEndpoint = '\0';
  char * phrase = &buffer[0];
  *phrase = '\0';

  strncat(pEndpoint, this->EEData->api_endpoint, ENDPOINT_MAX_LENGTH - strlen(pEndpoint));
  strncat(pEndpoint, phraseEndpoint, ENDPOINT_MAX_LENGTH - strlen(pEndpoint));

  Serial.println(pEndpoint);
  #ifndef NO_WIFI
  httpCli.begin(pEndpoint);
  httpCli.addHeader("Content-Type", "application/json");
  httpCli.addHeader("Authorization", pBearer);

  int httpCode = httpCli.GET();
  Serial.println(httpCode);

  if (httpCode != 200)
  {
    strncat(phrase, "Gehen Sie jetzt auf [WEBSITE], um hier Sätze anzeigen zu lassen!", HTTP_MAX_LEN - strlen(phrase));
    httpCli.end();
    this->pBearer = Login();
    return phrase;
  }
  // Serial.println(httpCli.getString());
  JSONVar jsonObject = JSON.parse(httpCli.getString().c_str());
  httpCli.end();

  if (JSON.typeof(jsonObject) == "undefined")
  {
    Serial.println("Parsing input failed!");
    strncat(phrase, "Gehen Sie jetzt auf [WEBSITE], um hier Sätze anzeigen zu lassen!", HTTP_MAX_LEN - strlen(phrase));
    return phrase;
  }

  appendPhrase(jsonObject, phrase);
#endif
  return phrase;
}

void http::appendPhrase(JSONVar &jsonObject, char *phrase)
{
  bool anonym = false;

  if (jsonObject.hasOwnProperty("anonym"))
  {
    anonym = (bool)jsonObject["anonym"];
  }

  if (!anonym)
  {
    strncat(phrase, ((const char *)jsonObject["creatorClassName"]), HTTP_MAX_LEN - strlen(phrase));
    strncat(phrase, ": ", HTTP_MAX_LEN - strlen(phrase));

    strncat(phrase, ((const char *)jsonObject["creatorFirstName"]), HTTP_MAX_LEN - strlen(phrase));
    strncat(phrase, ": ", HTTP_MAX_LEN - strlen(phrase));
  }
  strncat(phrase, ((const char *)jsonObject["phrase"]), HTTP_MAX_LEN - strlen(phrase));
}

char *http::Login()
{
  #ifndef NO_WIFI
  char *pEndpoint = &endpoint[0];
  *pEndpoint = '\0';

  strncat(pEndpoint, this->EEData->api_endpoint, ENDPOINT_MAX_LENGTH - strlen(pEndpoint));
  strncat(pEndpoint, "/api/v01/Login", ENDPOINT_MAX_LENGTH - strlen(pEndpoint));

  httpCli.begin(pEndpoint);

  httpCli.addHeader("Content-Type", "application/json");
#endif
  char *pBearer = &bearerBuffer[0];
  *pBearer = '\0';
#ifndef NO_WIFI
  JSONVar doc;

  doc["userName"] = this->EEData->api_username;
  doc["password"] = this->EEData->api_password;

  int httpCode = httpCli.POST(JSON.stringify(doc));
  if (httpCode > 0)
  {

    // file found at server
    if (httpCode == HTTP_CODE_OK)
    {
      const char *payload = httpCli.getString().c_str();
      JSONVar jsonObject = JSON.parse(payload);

      strncat(pBearer, "Bearer ", (HTTP_MAX_LEN - strlen(pBearer)));
      strncat(pBearer, ((const char *)jsonObject["args"]["Bearer"]), (HTTP_MAX_LEN - strlen(pBearer)));
    }
  }
  else
  {
    Serial.printf("[HTTP] POST... failed, error: %s\n", httpCli.errorToString(httpCode).c_str());
  }
  httpCli.end();
  #endif
  return pBearer;
}
