#include <Adafruit_NeoPixel.h>

#include "distance.h"
#include "json.h"


extern float last_temp;
extern bool isHotspot, isOccupied, isPoolAccessRefused;
extern int LED_BUILTIN;
extern char MAC[18];
extern char ident[50];

const int POOL_NB_VALUES = 30;
float poolTempValues[POOL_NB_VALUES];
String poolMacAddresses[POOL_NB_VALUES];
int poolIndex = 0;

void processTemperature(float temp, String mac) {
    poolTempValues[poolIndex] = temp;
    poolMacAddresses[poolIndex] = mac;
    poolIndex = (poolIndex + 1) % POOL_NB_VALUES;

    float maxTemp = last_temp;
    String maxMac = MAC;

    for (int i = 0; i < POOL_NB_VALUES; i++) {
        if (poolTempValues[i] > maxTemp) {
            maxTemp = poolTempValues[i];
            maxMac = poolMacAddresses[i];
        }
    }

    isHotspot = (maxTemp == last_temp) || (maxMac == MAC);
    digitalWrite(LED_BUILTIN, isHotspot ? HIGH : LOW);
    Serial.println(isHotspot ? "I'm the hotpost" : "I'm not the hotpost");
}

void handlePoolGlobalTopic(String message) {
    StaticJsonDocument<1500> jsonDoc;
    deserializeJson(jsonDoc, message);

    if (jsonDoc["location"]["gps"]) {
        double latitude = jsonDoc["location"]["gps"]["lat"];
        double longitude = jsonDoc["location"]["gps"]["lon"];
        double distance = getDistance(longitude, latitude);

        if (distance < 10.0) {
            float temp = jsonDoc["status"]["temperature"];
            String mac = jsonDoc["net"]["mac"];
            processTemperature(temp, mac);
        }
    }
}

void handlePoolPrivateTopic(String message) {
    StaticJsonDocument<1500> jsonDoc;
    deserializeJson(jsonDoc, message);

    if (jsonDoc["piscine"] && jsonDoc["info"]["ident"] == String(ident)) {
        isOccupied = jsonDoc["piscine"]["occuped"];
        isPoolAccessRefused = jsonDoc["piscine"]["access"] == "denied" ? true : false;
    }
}
