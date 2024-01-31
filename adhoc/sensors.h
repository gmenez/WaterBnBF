void setupSensors();

int getFanSpeed(float temperature);
bool handleTemperature(float temperature, bool isFireDetected);
void turnOnLedRingBasedOnTemperature(float temperature);
bool isFireDetected(float temperature, int lightValues[100]);
void triggerFirePreventSystem();
void processSensors();
