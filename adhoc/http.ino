#include <HTTPClient.h>

/**
 * Makes a HTTP POST request
*/
bool httpPOSTRequest(const char* UrlPath, const char* payload) {
  HTTPClient http;
  http.begin(UrlPath);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(payload);
  String response = http.getString();
  http.end();
  return httpResponseCode == 200;
} 
