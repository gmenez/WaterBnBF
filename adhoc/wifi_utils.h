/*** Basic/Static Wifi connection
     Fichier wificonnect/wifi_utils.h 
***/

// https://www.arduino.cc/en/Reference/WiFi
#include <WiFi.h> 
#include <WiFiMulti.h>
// Connection may need several tries 
// Time in ms for save disconnection, => delay between try
// cf https://github.com/espressif/arduino-esp32/issues/2501  
#define SaveDisconnectTime 1000 
#define WiFiMaxTry 10
String translateEncryptionType(wifi_auth_mode_t encryptionType);
void wifi_printstatus(int C);
bool wifi_is_connected();
void wifi_connect_basic(String hostname, String ssid, String passwd);
int wifi_search_neighbor();
void wifi_connect_multi(String hostname);
