#include "json.h"

/**
 * Make a JSON from the status
*/
StaticJsonDocument<1500> makeEspModelJSON(esp_model *em) {
  StaticJsonDocument<1500> doc;

  JsonObject status = doc.createNestedObject("status");
  status["temperature"] = em->temperature;
  status["light"] = em->luminosity;
  status["regul"] = em->regulationState ? "RUNNING" : "HALT";
  status["fire"] = em->fireDetected;
  status["heat"] = em->heaterState ? "ON" : "OFF";
  status["cold"] = em->coolerState ? "ON" : "OFF";

  JsonObject location = doc.createNestedObject("location");
  location["room"] = em->room;
  JsonObject gps = location.createNestedObject("gps");
  gps["lat"] = em->latitude;
  gps["lon"] = em->longitude;
  location["address"] = em->address;

  JsonObject regul = doc.createNestedObject("regul");
  regul["lt"] = em->lowThreshold;
  regul["ht"] = em->highThreshold;

  JsonObject info = doc.createNestedObject("info");
  info["ident"] = em->ident;
  info["loc"] = em->loc;
  info["user"] = em->user;

  JsonObject net = doc.createNestedObject("net");
  net["uptime"] = em->uptime;
  net["ssid"] = em->WiFiSSID;
  net["mac"] = em->MAC;
  net["ip"] = em->IP;

  JsonObject reporthost = doc.createNestedObject("reporthost");
  reporthost["target_ip"] = em->target_ip;
  reporthost["target_port"] = em->target_port;
  reporthost["sp"] = em->target_sp;

  JsonObject piscine = doc.createNestedObject("piscine");
  piscine["hotspot"] = em->hotspot;
  piscine["occuped"] = em->occuped;

  return doc;
}

/**
 * Make a JSON of the status
*/
StaticJsonDocument<1500> makeEspStatusJSON(esp_status *es) {
  StaticJsonDocument<1500> doc;

  if (es->temperature && es->temperature != -2146887768) {
    doc["temperature"] = es->temperature;
  }
  if (es->luminosity) {
    doc["light"] = es->luminosity;
  }
  if (es->uptime) {
    doc["uptime"] = es->uptime;
  }
  if (es->heaterState) {
    doc["heat"] = es->heaterState;
  }
  if (es->coolerState) {
    doc["cold"] = es->coolerState;
  }
  if (es->ht && es->ht != -2146887768) {
    doc["ht"] = es->ht;
  }
  if (es->lt && es->lt != -2146887768) {
    doc["lt"] = es->lt;
  }

  return doc;
}

/**
 * Handle a message from the serial input and update the thresholds
*/
void handleMsgFromSerialInput() {
  if(Serial.available()) {
    StaticJsonDocument<200> jsonDoc;
    String payload = Serial.readStringUntil('\n');
    deserializeJson(jsonDoc, payload);
    if (jsonDoc.containsKey("high") || jsonDoc.containsKey("low")) {
      if (jsonDoc.containsKey("high")) {
        TRESHOLD_HIGH = jsonDoc["high"];
      } else if (jsonDoc.containsKey("low")) {
        TRESHOLD_LOW = jsonDoc["low"];
      }
    }
  }
}
