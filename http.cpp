#include "http.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <assert.h>

#define DEBUG_OUT
//#undef DEBUG_OUT

// Variablen

char buffer[HTTP_MAX_LEN + 1] = { '\0' };

http::http(char* endpoint) {
  this->endpoint = endpoint;
}

http::http(char* endpoint, char* token) {
  this->endpoint = endpoint;
  this->token = token;
}

http::http(char* endpoint, char* testJSON, int bum) {
  this->endpoint = endpoint;
  this->testJSON = testJSON;
}
http::~http() {
}

char* http::GetPhrase() {

  return http::Get();
}

char* http::Get() {
  char* phrase = &buffer[0];
  buffer[0] = '\0';
  HTTPClient httpCli;
  Serial.println(endpoint);
  httpCli.begin(endpoint);  //HTTP
  int httpCode = httpCli.GET();

  JSONVar jsonObject = JSON.parse(httpCli.getString().c_str());
  if (JSON.typeof(jsonObject) == "undefined") {
    Serial.println("Parsing input failed!");
  } else {
    Serial.println(jsonObject);
    if (jsonObject.hasOwnProperty("class")) {
      
      strncat(phrase, ((const char*)jsonObject["class"]), (HTTP_MAX_LEN - strlen(phrase)));
    }
    if (jsonObject.hasOwnProperty("firstName")) {
      strncat(phrase, ((const char*)jsonObject["firstName"]), (HTTP_MAX_LEN - strlen(phrase)));
    }
    if (jsonObject.hasOwnProperty("phrase")) {
      strncat(phrase, ((const char*)jsonObject["phrase"]), (HTTP_MAX_LEN - strlen(phrase)));
    }
  }
  httpCli.end();
#ifdef DEBUG_OUT  
  Serial.print("  ---> Ergebnis: ");
  Serial.println(phrase);
#endif  
  return phrase;
}