/*
  Vasseur Pierre-Adrien
  Master M1 Info Alternance
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "DallasTemperature.h"
#include "json.h"
#include "http.h"


/**
  * Declare wifi variables
**/

char uptime[50] = "";
char WiFiSSID[50] = "";
char MAC[18] = "";
char IP[16] = "";

#include "wifi_utils.h"

/**
 * Declare identity variables
*/
char ident[50] = "P_22012379";

/**
  * Declare sensors, leds...
**/

int SENSORS_UPDATE_INTERVAL = 2000;

// DHT Temperature Sensor
#include "DHT.h"
#define DHTPIN 23 // sensor pin
#define DHTTYPE DHT11 // sensor type
DHT dht(DHTPIN, DHTTYPE); // constructor

// Air-Conditioner led pin
int ACLED = 19;

// Header led pin
int HLED = 21;

// Fan
int FAN = 27;

// Onboard led
int LED_BUILTIN = 2;

// Led ring
int LED_PIN = 12;
int NUMLEDS = 5;
Adafruit_NeoPixel strip(NUMLEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Status variables
float last_temp = 0;
int last_light = 0;
bool isRegulatingTemperature = false;
bool fireDetected = false;
bool isInCoolingMode = false;
bool isInHeatingMode = false;
String target_ip = "";
int target_port = 1880;
int target_sp = 0;
String target_status = "NOT CONNECTED";
bool isHotspot = false;
bool isOccupied = false;
bool isPoolAccessRefused = false;

#include "sensors.h"

/**
  * Declare program variables
**/

int TRESHOLD_HIGH = 22; // temperature in °C -> if up than this value, turn on the air conditioner
int TRESHOLD_LOW = 20; // temperature in °C -> if down than this value, turn off the fan and turn on the heater
int LIGHT_TRESHOLD = 2000; // light value -> if up than this value, turn on the built-in led
int LIGHT_NUM_VALUES = 5; // number of light values to take into account to detect a fire
int POOL_REFUSED_TIMEOUT = 10; // time in seconds to wait before returning to previous led string color

int lightValues[100]; // array of light values used to detect a fire
int i = 0; // index of the lightValues array


/**
  * Declare web server
**/

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


/**
 * Declare MQTT variables
*/

// Brooker
//const char* mqtt_server = "192.168.1.101"; 
const char* mqtt_server = "test.mosquitto.org";

// Topics
String TOPIC_POOL_GLOBAL = "uca/iot/piscine";
String TOPIC_POOL_PRIVATE = "uca/iot/piscine/22012379"; // used to exchange config data

// ESP32 MQTT client
WiFiClient espClient;
PubSubClient mqttclient(espClient);

#include "mqtt.h"

/**
 * Declare location variables
*/
char address[200] = "Les lucioles";
double latitude = 43.62454;
double longitude = 7.050628;

#include "distance.h"

/**
  * Use serial to communicate with the ESP32
**/
#define USE_SERIAL


/**
 * SHOULD USE HTTP OR MQTT
*/
// #define USE_HTTP
#define USE_MQTT





/**
   Process sensors
**/
void updateSensors(int delai) {
  static uint32_t tick = 0;
  if (millis() - tick < delai) {
    return;
  } else {
    tick = millis();
    processSensors();
  }
}

void sendEspModelHTTP(StaticJsonDocument<1500> *doc) {
  String url = "http://";
  url += target_ip;
  url += ":";
  url += target_port;
  url += "/esp?mac=";
  url += MAC;

  bool isPostOk = httpPOSTRequest(url.c_str(), doc->as<String>().c_str());
  target_status = isPostOk ? "CONNECTED" : "CONNECTION FAILED";

  #ifdef USE_SERIAL
    Serial.println("Response : " + isPostOk);
  #endif

  #ifdef USE_SERIAL
    serializeJson(*doc, Serial);
    Serial.println("");
  #endif
}

/**
   Send a periodic report to the server every target_sp seconds
*/
void sendPeriodicReport() {
  static uint32_t tick = 0;

  // Do not send a report if no delay is set
  if (target_sp == 0) {
    return;
  }

  if (millis() - tick < target_sp * 1000) {
    return;
  } else {
    tick = millis();
    esp_model em;
    em.luminosity = last_light;
    em.temperature = last_temp;
    em.highThreshold = TRESHOLD_HIGH;
    em.lowThreshold = TRESHOLD_LOW;
    em.coolerState = isInCoolingMode;
    em.heaterState = isInHeatingMode;
    em.regulationState = isRegulatingTemperature;
    em.fireDetected = fireDetected;

    strncpy(em.uptime, String(millis() / 1000).c_str(), sizeof(em.uptime));
    strncpy(em.WiFiSSID, WiFiSSID, sizeof(em.WiFiSSID));
    strncpy(em.MAC, MAC, sizeof(em.MAC));
    strncpy(em.IP, IP, sizeof(em.IP));

    strncpy(em.target_ip, target_ip.c_str(), sizeof(em.target_ip));
    em.target_port = target_port;
    em.target_sp = target_sp;

    em.latitude = latitude;
    em.longitude = longitude;

    em.hotspot = isHotspot;
    em.occuped = isOccupied;

    strncpy(em.ident, ident, sizeof(em.ident));

    StaticJsonDocument<1500> doc = makeEspModelJSON(&em);

    #ifdef USE_HTTP
      sendEspModelHTTP(&doc);
    #endif

    #ifdef USE_MQTT
      Serial.println("Sending report to MQTT topic : " + TOPIC_POOL_GLOBAL);
      mqttclient.publish(TOPIC_POOL_GLOBAL.c_str(), doc.as<String>().c_str());
    #endif
  }
}











void setup() {
  Serial.begin(9600);
  while (!Serial); // wait for a serial connection

  /* WiFi connection  -----------------------*/
  String hostname = "ESP32";
  wifi_connect_multi(hostname);

  /* WiFi status    --------------------------*/
  if (wifi_is_connected()) {
    Serial.print("\nWiFi connected : yes ! \n");
    wifi_printstatus(0);

    // MQTT setup
    #ifdef USE_MQTT
      mqttclient.setServer(mqtt_server, 1883);
      mqttclient.setBufferSize(1024);
      mqttclient.setCallback(mqttCallback); 
      target_sp = 10;
    #endif
  } else {
    Serial.print("\nWiFi connected : no ! \n");
    return;
  }

  /* Setup sensors, leds...  ------------------*/
  setupSensors();

  /* SPIFFS  --------------------------*/
  SPIFFS.begin(true);

  /* WebServer  --------------------------*/
  setup_http_routes(&server);
  server.begin();

  /* Light setup  --------------------------*/
  for (int i = 0; i < LIGHT_NUM_VALUES; i++) {
    lightValues[i] = 0;
  }
}

void loop() {
  updateSensors(SENSORS_UPDATE_INTERVAL);
  #ifdef USE_SERIAL
    handleMsgFromSerialInput();
  #endif

  #ifdef USE_MQTT
    mqttSubscribeToTopics();
  #endif

  sendPeriodicReport();

  #ifdef USE_MQTT
    mqttclient.loop();
  #endif
}
