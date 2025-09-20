#include "DynamicAdaptiveFilterV2.h"
#include "params/params_analog.h"

// Poti4: GPIO19, Threshold 2 %, EMA, Länge 10, 10 Hz, Decay 1 h, Warm-up 1 s
FILTER_ANALOG(19, 2.0f, EMA, 10, 10.0f, 3600000, 1000);

DynamicAdaptiveFilterV2 filter(filter_analog_19);

void setup() {
  Serial.begin(115200);
  pinMode(19, INPUT); // Poti4 an GPIO19
  Serial.println("Analog Filter für Poti4 gestartet...");
}

void loop() {
  // Poti4 auslesen (0–4095, 12-bit ADC)
  int adcValue = analogRead(19);
  float potiValue = (adcValue / 4095.0f) * 100.0f; // Skaliere auf 0–100 %

  // SensorData für Filter
  SensorData data;
  data.values = {potiValue};
  data.timestamp = millis();
  data.sensorId = "POTI4";
  filter.pushSensorData(data);

  // Gefilterten Wert ausgeben
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 100) { // 10 Hz
    auto filtered = filter.getFilteredValues();
    Serial.printf("Poti4: Raw = %.1f%%, Filtered = %.1f%%\n", potiValue, filtered[0]);
    lastPrint = millis();
  }

}
