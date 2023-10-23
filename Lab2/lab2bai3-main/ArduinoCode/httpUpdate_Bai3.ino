#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>


#ifndef APSSID
#define APSSID "UiTiOt-E3.1"
#define APPSK "UiTiOtAP"
#endif

struct Config {
  String name;
  int version;
};
Config config;
Config newConfig;

ESP8266WiFiMulti WiFiMulti;
const char *configFilename   = "/config.json";

void readFile(const char *filename){
  File file = LittleFS.open(filename, "r");
  if (!file) {
    Serial.println("Failed to open data file");
    return;
  }
  while (file.available()) {
    Serial.print((char)file.read());
  }
  Serial.println();
  file.close();
}

void saveConfiguration(const char *filename, const Config &config){
  File file = LittleFS.open(filename, "w+");
  if (!file) {
    return;
  }
  StaticJsonDocument<100> doc;
  doc["name"] = config.name;
  doc["version"] = config.version;

  if (serializeJson(doc, file) == 0){
    Serial.println("Failed to write to file");
  }

  file.close(); 
}

void loadConfiguration(const char *filename, Config &config){
  File file = LittleFS.open(filename, "r");
  StaticJsonDocument<100> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error){
    Serial.println("Failed to read file, using default configuration");
  }

  config.name = (String)doc["name"];
  config.version  = doc["version"];
  file.close();
}



void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(false);

  Serial.println();

  ESPhttpUpdate.setClientTimeout(2000);  // default was 8000

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(APSSID, APPSK);
}

void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}


void loop() {
  Serial.println("Flash config is:");
  loadConfiguration(configFilename, config);
  readFile(configFilename);
// wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;
    http.begin(client, "http://172.31.10.24:3000/config.json");
    int httpCode = http.GET();
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

    // Add optional callback notifiers
    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);

    if (httpCode == 200){
      String response = http.getString();
      const char* serverConfig = response.c_str();
      Serial.println(serverConfig);
      DynamicJsonDocument doc(response.length());
      deserializeJson(doc, serverConfig);

      if (doc["name"] != config.name || doc["version"] != config.version) {
      // Tải file binary từ server
        newConfig.name = (String)doc["name"];
        newConfig.version = doc["version"];
        saveConfiguration(configFilename, newConfig);
        readFile(configFilename);
        Serial.println(newConfig.version);
        Serial.println(newConfig.name);
        t_httpUpdate_return ret = ESPhttpUpdate.update(client, "http://172.31.10.24:3000/update/1698045050581.bin");
        switch (ret) {
        case HTTP_UPDATE_FAILED: Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str()); break;

        case HTTP_UPDATE_NO_UPDATES: Serial.println("HTTP_UPDATE_NO_UPDATES"); break;

        case HTTP_UPDATE_OK: Serial.println("HTTP_UPDATE_OK"); break;
      }
    }
  }
}
}
