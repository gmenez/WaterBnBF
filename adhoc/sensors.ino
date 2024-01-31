#include "DHT.h"
#include <Adafruit_NeoPixel.h>
#include "sensors.h"

extern DHT dht;
extern int ACLED, HLED, FAN, LED_BUILTIN, LED_PIN;
extern Adafruit_NeoPixel strip;
extern int TRESHOLD_HIGH, TRESHOLD_LOW, LIGHT_TRESHOLD, LIGHT_NUM_VALUES, POOL_REFUSED_TIMEOUT;
extern int lightValues[100];

extern float last_temp;
extern int last_light;
extern bool isRegulatingTemperature, fireDetected, isInCoolingMode, isInHeatingMode, isOcccupied, isPoolAccessRefused;


/**
 * Sets up sensors
*/
void setupSensors() {
  dht.begin();
  // LED 
  pinMode(ACLED, OUTPUT);
  pinMode(HLED, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  // FAN
  ledcAttachPin(FAN, 0);
  ledcSetup(0, 25000, 8);
  ledcWrite(0,255);
  // Led ring
  strip.begin();
}

/**
 * Returns the fan speed based on the temperature
*/
int getFanSpeed(float temperature) {
  int rcv[5] = {0, 64, 127, 191, 255};
  if (temperature < (TRESHOLD_HIGH + 0.5)) { 
    return rcv[1];
  } else if (temperature >= (TRESHOLD_HIGH + 0.5) && temperature < (TRESHOLD_HIGH + 1)) {
    return rcv[2];
  } else if (temperature >= (TRESHOLD_HIGH + 1) && temperature < (TRESHOLD_HIGH + 1.5)) {
    return rcv[3];
  } else {
    return rcv[4];
  }
}

/**
 * Handles the temperature
*/
bool handleTemperature(float temperature, bool isFireDetected) {
  if (temperature > TRESHOLD_HIGH && !isFireDetected) {
    digitalWrite(ACLED, HIGH);
    digitalWrite(HLED, LOW);
    ledcWrite(0, getFanSpeed(temperature));
    return true;
  } else if (temperature < TRESHOLD_LOW) {
    digitalWrite(ACLED, LOW);
    digitalWrite(HLED, HIGH);
    ledcWrite(0, 0);
    return true;
  } else {
    digitalWrite(ACLED, LOW);
    digitalWrite(HLED, LOW);
    ledcWrite(0, 0);
    return false;
  }
}

/**
 * Turns on the led ring based on the temperature
*/
void turnOnLedRingBasedOnTemperature(float temperature) {
  strip.clear();
  if (temperature > TRESHOLD_HIGH) {
    for (int i = 0; i < temperature - TRESHOLD_HIGH; i++) {
      if (i < NUMLEDS) {
        strip.setPixelColor(i, strip.Color(255, 0, 0)); // red
      } else {
        break;
      }
    }
  } else if (temperature < TRESHOLD_LOW) {
    strip.setPixelColor(0, strip.Color(0, 0, 255)); // blue
  } else {
    strip.setPixelColor(0, strip.Color(0, 255, 0)); // green
  }
}

/**
 * Returns true if a fire is detected
*/
bool isFireDetected(float temperature, int lightValues[100]) {
  int lightValuesSum = 0;
  for (int i = 0; i < LIGHT_NUM_VALUES; i++) {
    lightValuesSum += lightValues[i];
  }
  float lightValuesAverage = lightValuesSum / LIGHT_NUM_VALUES;
  if (temperature > TRESHOLD_HIGH && lightValuesAverage > LIGHT_TRESHOLD) {
    return true;
  }
  return false;
}

/**
 * Triggers the fire prevent system
*/
void triggerFirePreventSystem() {
  digitalWrite(LED_BUILTIN, HIGH); // turn on the built-in led
  ledcWrite(0, 0); // turn off the fan
  strip.clear(); // clear the led ring
  for (int i = 0; i < NUMLEDS; i++) {
    strip.setPixelColor(i, strip.Color(255, 0, 0)); // red
  }
}

/**
 * Processes sensors and update variables
*/
void processSensors() {
  fireDetected = false;
  isRegulatingTemperature = false;
  int lightSensorValue = analogRead(A5); // GPIO 33
  float t = dht.readTemperature();

  if (isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  lightValues[i] = lightSensorValue;
  i++;
  if (i == LIGHT_NUM_VALUES) {
    i = 0;
  }

  fireDetected = isFireDetected(t, lightValues);

  isRegulatingTemperature = handleTemperature(t, fireDetected);
  turnOnLedRingBasedOnTemperature(t);

  if (fireDetected) {
    triggerFirePreventSystem();
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }


  last_temp = t;
  last_light = lightSensorValue;
  isInCoolingMode = (t > TRESHOLD_HIGH);
  isInHeatingMode = (t < TRESHOLD_LOW);

  int stripColor; 

  if (isOccupied) {
    stripColor = strip.Color(255, 255, 0); // yellow
  }
  if (isPoolAccessRefused) {
    stripColor = strip.Color(255, 0, 0); // red
  }
  if (!isOccupied && !isPoolAccessRefused) {
    stripColor = strip.Color(127,255,0); // green
  }

  strip.clear(); // clear the led ring
  for (int i = 0; i < NUMLEDS; i++) {
    strip.setPixelColor(i, stripColor);
  }

  strip.show();

  POOL_REFUSED_TIMEOUT--;
  if (POOL_REFUSED_TIMEOUT <= 0) {
    POOL_REFUSED_TIMEOUT = 10;
    isPoolAccessRefused = false;
  }
}
