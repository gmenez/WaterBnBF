#include <Adafruit_NeoPixel.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "pool.h"


extern String TOPIC_POOL_GLOBAL, TOPIC_POOL_PRIVATE;
extern PubSubClient mqttclient;


void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Message arrived on topic : " + String(topic));

  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (String(topic) == TOPIC_POOL_GLOBAL) {
    handlePoolGlobalTopic(message);
  } else if (String(topic) == TOPIC_POOL_PRIVATE) {
    handlePoolPrivateTopic(message);
  }
}

void mqttSubscribeToTopics() {
  while (!mqttclient.connected()) { 
    Serial.println("Attempting MQTT connection...");
    
    String mqttclientId = "ESP32-pa";
    mqttclientId += WiFi.macAddress();

    if (mqttclient.connect(mqttclientId.c_str(), NULL, NULL)) {
      bool rsGlobalChannel = mqttclient.subscribe(TOPIC_POOL_GLOBAL.c_str(), 1);
      bool rsPrivateChannel = mqttclient.subscribe(TOPIC_POOL_PRIVATE.c_str(), 1);

      if (rsGlobalChannel && rsPrivateChannel) {
        Serial.println("subscribed to topics");
      } else {
        Serial.println("failed to subscribe to topics");
      }

    } else {
      Serial.println("failed, rc=");
      Serial.println(mqttclient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000); 
    }
  }
}
