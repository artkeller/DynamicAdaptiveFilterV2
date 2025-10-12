#include <DynamicAdaptiveFilterV2.h>
#include "params_analog.h"

#define USE_LMS
FILTER_ANALOG(1, 2.0f, LMS, 5, nullptr, 0, 10.0f, 3600000, 1000); // Setzt die Parameter für 'filter_analog_1' 

DynamicAdaptiveFilterV2 filter(filter_analog_1);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  filter.begin();
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Setup complete");
}

void loop() {
  SensorData data;
  data.values = {analogRead(A0) / 4095.0f * 3.3f}; // Beispiel: ADC-Wert
  data.timestamp = millis();
  data.sensorId = "TEST";
  if (!filter.pushSensorData(data)) {
    Serial.println("Filter error");
  }
  delay(1000);
}
