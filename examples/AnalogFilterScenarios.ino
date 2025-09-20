#include "DynamicAdaptiveFilterV2.h"
#include "params_analog.h"

// Filter für Poti4 (GPIO19)
FILTER_ANALOG(19, 2.0f, EMA, 10, nullptr, 0, 10.0f, 3600000, 1000);

// Kombinierte Filterkonfiguration (z. B. Poti4 + Vbus an GPIO18)
FILTER_ANALOG(18, 1.0f, EMA, 10, nullptr, 0, 10.0f, 3600000, 1000); // Vbus
std::vector<FilterConfig> configs = {
  filter_analog_19[0], // Poti4
  filter_analog_18[0]  // Vbus
};

DynamicAdaptiveFilterV2 filter(configs);

void setup() {
  Serial.begin(115200);
  pinMode(19, INPUT); // Poti4
  pinMode(18, INPUT); // Vbus
  Serial.println("Analog Filter für Szenarien gestartet...");
}

void loop() {
  // Poti4 (0–3.3 V -> 0–100 %)
  int adcPoti = analogRead(19);
  float potiValue = (adcPoti / 4095.0f) * 100.0f;

  // Vbus (0–3.3 V -> 0–5 V, Spannungsteiler)
  int adcVbus = analogRead(18);
  float vbusValue = (adcVbus / 4095.0f) * 5.0f;

  // SensorData
  SensorData data;
  data.values = {potiValue, vbusValue};
  data.timestamp = millis();
  data.sensorId = "ANALOG";
  filter.pushSensorData(data);

  // Ausgabe (10 Hz)
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 100) {
    auto filtered = filter.getFilteredValues();
    Serial.printf("Poti4: Raw = %.1f%%, Filtered = %.1f%% | Vbus: Raw = %.2fV, Filtered = %.2fV\n",
                  potiValue, filtered[0], vbusValue, filtered[1]);
    lastPrint = millis();
  }
}