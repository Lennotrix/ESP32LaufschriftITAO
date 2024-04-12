#include "http.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

// #define DEBUG_OUT
// #undef DEBUG_OUT

// Variablen

char buffer[HTTP_MAX_LEN + 1] = {'\0'};
char bearerBuffer[HTTP_MAX_LEN + 1] = {'\0'};

HTTPClient httpCli;

http::http(const char *uName, const char *pw)
{
  this->uName = uName;
  this->uPw = pw;
  this->pBearer = Login();
}

http::~http()
{
}

const char *const kEndpoint = "http://10.1.1.30:5047/api/v01/phrase?id=5";

char *http::GetPhrase()
{
  char *phrase = &buffer[0];
  *phrase = '\0';

  Serial.println(kEndpoint);

  httpCli.begin(kEndpoint);
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
  const char *endpoint = "http://10.1.1.30:5047/api/v01/auth/Login";

  httpCli.begin(endpoint);

  httpCli.addHeader("Content-Type", "application/json");

  char *pBearer = &bearerBuffer[0];
  *pBearer = '\0';

  JSONVar doc;

  doc["userName"] = this->uName;
  doc["password"] = this->uPw;

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
  return pBearer;
  httpCli.end();
}
