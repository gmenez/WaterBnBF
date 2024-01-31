/** JSON methods **/

#ifndef MAKEJSONH
#define MAKEJSONH
#include <ArduinoJson.h>

typedef struct {
  int luminosity;
  float temperature;
  float highThreshold;
  float lowThreshold;
  bool coolerState; // "ON" : "OFF";
  bool heaterState; // "ON" : "OFF";
  bool regulationState; // Regulation status : "RUNNING" or "HALT"
  bool fireDetected;
  // Location
  double latitude = 43.62454;
  double longitude = 7.050628;
  const char room[50] = "TD06";
  char address[200] = "Les lucioles";
  // Network
  char uptime[50] = "";
  char WiFiSSID[50] = "";
  char MAC[18] = "";
  char IP[16] = "";
  // Reporting
  char target_ip[16] = "";
  int target_port = 1880;
  int target_sp = 2;
  // Info
  char ident[50] = "P_22012379";
  const char loc[50] = "A Biot";
  const char user[50] = "PA";
  // Piscine
  bool hotspot = false;
  bool occuped = false;
} esp_model;

typedef struct {
  int temperature;
  float luminosity;
  int uptime;
  bool heaterState;
  bool coolerState;
  int ht;
  int lt;
} esp_status;

#endif

StaticJsonDocument<1500> makeEspModelJSON(esp_model *em);
// StaticJsonDocument<1500> makeEspStatusJSON(esp_status *es);
void handleMsgFromSerialInput();
