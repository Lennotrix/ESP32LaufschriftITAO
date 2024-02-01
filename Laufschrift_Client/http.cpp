#include "http.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

//#define DEBUG_OUT
//#undef DEBUG_OUT

// Variablen

char buffer[HTTP_MAX_LEN + 1] = { '\0' };
char bearerBuffer[HTTP_MAX_LEN + 1] = { '\0' };

http::http(const char* uName, const char* pw) {
  this->uName = uName;
  this->uPw = pw;
  this->pBearer = Login();
}

http::~http() {
}

char* http::GetPhrase() {
  const char* endpoint = "http://10.1.1.30:5047/api/v01/phrase/device";
  char* phrase = &buffer[0];
  *phrase = '\0';
  bool anonym;

  HTTPClient httpCli;
  Serial.println(endpoint);
  httpCli.begin(endpoint);  //HTTP
  httpCli.addHeader("Content-Type", "application/json");
  httpCli.addHeader("Authorization", pBearer);

  int httpCode = httpCli.GET();
  Serial.println(httpCode);
  if (httpCode == 401) {
    this->pBearer = Login();
    strncat(phrase, ("401"), (HTTP_MAX_LEN - strlen(phrase)));
    return phrase;
  }
  else if(httpCode != 200){
    strncat(phrase, "Gehen Sie jetzt auf [WEBSITE] um hier Sätze anzeigen zu lassen!", (HTTP_MAX_LEN - strlen(phrase)));
    return phrase;
  }
  JSONVar jsonObject = JSON.parse(httpCli.getString().c_str());
  if (JSON.typeof(jsonObject) == "undefined") {
    Serial.println("Parsing input failed!");
    return phrase;
  }
  Serial.println(jsonObject);
  if (jsonObject.hasOwnProperty("anonym")) {
    anonym = (bool)jsonObject["anonym"];
  }
  if (!anonym) {
    if (jsonObject.hasOwnProperty("creatorClassName")) {

      strncat(phrase, ((const char*)jsonObject["creatorClassName"]), (HTTP_MAX_LEN - strlen(phrase)));
      strncat(phrase, ": ", (HTTP_MAX_LEN - strlen(phrase)));
    }
    if (jsonObject.hasOwnProperty("creatorFirstName")) {
      strncat(phrase, ((const char*)jsonObject["creatorFirstName"]), (HTTP_MAX_LEN - strlen(phrase)));
      strncat(phrase, ": ", (HTTP_MAX_LEN - strlen(phrase)));
    }
  }
  if (jsonObject.hasOwnProperty("phrase")) {
    strncat(phrase, ((const char*)jsonObject["phrase"]), (HTTP_MAX_LEN - strlen(phrase)));
  }

  httpCli.end();

  return phrase;
}

char* http::Login() {
  const char* endpoint = "http://10.1.1.30:5047/api/v01/auth/Login";
  HTTPClient http;

  http.begin(endpoint);

  http.addHeader("Content-Type", "application/json");

  char* pBearer = &bearerBuffer[0];
  *pBearer = '\0';

  JSONVar doc;

  doc["userName"] = this->uName;
  doc["password"] = this->uPw;

  int httpCode = http.POST(JSON.stringify(doc));

  if (httpCode > 0) {

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      const char* payload = http.getString().c_str();
      JSONVar jsonObject = JSON.parse(payload);

      strncat(pBearer, "Bearer ", (HTTP_MAX_LEN - strlen(pBearer)));
      strncat(pBearer, ((const char*)jsonObject["args"]["Bearer"]), (HTTP_MAX_LEN - strlen(pBearer)));
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
  return pBearer;
}
