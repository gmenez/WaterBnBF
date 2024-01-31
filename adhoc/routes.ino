#include "ESPAsyncWebServer.h"
#include "routes.h"
#include "SPIFFS.h"
#include "json.h"

#define USE_SERIAL Serial
extern int TRESHOLD_HIGH, TRESHOLD_LOW, LIGHT_TRESHOLD;
extern float last_temp;
extern int last_light;
extern char WiFiSSID[50];
extern char MAC[18];
extern char IP[16];
extern bool isInCoolingMode, isInHeatingMode;
extern String target_ip;
extern int target_port, target_sp;
extern char address[200];
extern String target_status;


/**
 * Process template variables
*/
String processor(const String& var){
  if (var == "TEMPERATURE") {
    return String(last_temp);
  } else if (var == "LIGHT") {
    return String(last_light);
  } else if (var == "UPTIME") {
    return String(millis() / 1000);
  } else if (var == "SSID") {
    return String(WiFiSSID);
  } else if (var == "MAC") {
    return String(MAC);
  } else if (var == "IP") {
    return String(IP);
  } else if (var == "LT") {
    return String(TRESHOLD_LOW);
  } else if (var == "HT") {
    return String(TRESHOLD_HIGH);
  } else if (var == "COOLER") {
    return String(isInCoolingMode);
  } else if (var == "HEATER") {
    return String(isInHeatingMode);
  } else if (var == "PRT_IP") {
    return target_ip;
  } else if (var == "PRT_PORT") {
    return String(target_port);
  } else if (var == "PRT_T") {
    return String(target_sp);
  } else if (var == "WHERE") {
    return String(address);
  } else {
    return String();
  }
}

/* 
* Sets up AsyncWebServer and routes 
*/
void setup_http_routes(AsyncWebServer* server) {
  server->serveStatic("/", SPIFFS, "/").setTemplateProcessor(processor);  
  
  // Declaring root handler, and action to be taken when root is requested
  auto root_handler = server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);   
  });
  
  /**
   * List of GET routes to retrieve updated sensor values
  */
  server->on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(last_temp).c_str());
  });
  server->on("/light", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(last_light).c_str());
  });
  server->on("/uptime", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(millis() / 1000).c_str());
  });
  server->on("/cooler", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(isInCoolingMode).c_str());
  });
  server->on("/heater", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(isInHeatingMode).c_str());
  });
  server->on("/ht", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(TRESHOLD_HIGH).c_str());
  });
  server->on("/lt", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(TRESHOLD_LOW).c_str());
  });
  server->on("/target_available", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", target_status.c_str());
  });
  server->on("/value", HTTP_GET, [](AsyncWebServerRequest *request){
    esp_status es;

    if (request->hasArg("temperature")) {
      es.temperature = last_temp;
    }
    if (request->hasArg("light")) {
      es.luminosity = last_light;
    }
    if (request->hasArg("uptime")) {
      es.uptime = millis() / 1000;
    }
    if (request->hasArg("cooler")) {
      es.coolerState = isInCoolingMode;
    }
    if (request->hasArg("heater")) {
      es.heaterState = isInHeatingMode;
    }
    if (request->hasArg("ht")) {
      es.ht = TRESHOLD_HIGH;
    }
    if (request->hasArg("lt")) {
      es.lt = TRESHOLD_LOW;
    }

    StaticJsonDocument<1500> doc = makeEspStatusJSON(&es);

    request->send_P(200, "application/json", doc.as<String>().c_str());
  });


  /** 
    * Set program parameters
  **/
  server->on("/set", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasArg("light_threshold")) {
      LIGHT_TRESHOLD = atoi(request->arg("light_threshold").c_str());
      request->send_P(200, "text/plain", "Threshold Set !");
    } else if (request->hasArg("ht")) {
      Serial.println(request->arg("ht").c_str());
      TRESHOLD_HIGH = atoi(request->arg("ht").c_str());
      request->send_P(200, "text/plain", "Temp Threshold High Set !");
    } else if (request->hasArg("lt")) {
      TRESHOLD_LOW = atoi(request->arg("lt").c_str());
      request->send_P(200, "text/plain", "Temp Threshold Low Set !");
    }
  });
  
  /** 
    * A route receiving a POST request with Internet coordinates 
    * of the reporting target host.
  **/
  server->on("/target", HTTP_POST, [](AsyncWebServerRequest *request){
    Serial.println("Receive Request for a periodic report !"); 
    if (request->hasArg("ip") && request->hasArg("port") && request->hasArg("sp")) {
      target_ip = request->arg("ip");
      target_port = atoi(request->arg("port").c_str());
      target_sp = atoi(request->arg("sp").c_str());
    }
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // If request doesn't match any route, returns 404.
  server->onNotFound([](AsyncWebServerRequest *request){
    request->send(404);
  });
}
/*===================================================*/
