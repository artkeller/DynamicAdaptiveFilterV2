#include "DynamicAdaptiveFilterV2.h"
#include "params/params_analog.h"

// Optional: Aktiviere LMS für Brummfilter
//#define USE_LMS

// Poti4: GPIO19, Threshold 2 %, EMA, Länge 10, 10 Hz, Decay 1 h, Warm-up 1 s
FILTER_ANALOG(19, 2.0f, EMA, 10, nullptr, 0, 10.0f, 3600000, 1000);

#if defined(USE_LMS)
const std::vector<FilterConfig> filter_lms = {
  {LMS, 10, nullptr, 0, 100.0f, 10000, 1000, 5.0f, 0.0f, VALUE_MODE, 0.01f}
};
DynamicAdaptiveFilterV2 filter({filter_analog_19[0], filter_lms[0]});
#else
DynamicAdaptiveFilterV2 filter(filter_analog_19);
#endif

void setup() {
  Serial.begin(115200);
  pinMode(19, INPUT); // Poti4 an GPIO19
  Serial.println("Analog Filter für Poti4 gestartet...");
}

void loop() {
  int adcValue = analogRead(19);
  float potiValue = (adcValue / 4095.0f) * 100.0f; // Skaliere auf 0–100 %

  SensorData data;
#if defined(USE_LMS)
  data = {{potiValue, potiValue}, {0.0f, potiValue}, millis(), "POTI4"};
#else
  data = {{potiValue}, {}, millis(), "POTI4"};
#endif
  if (filter.pushSensorData(data)) {
    auto filtered = filter.getFilteredValues();
    Serial.printf("Poti4: Raw = %.1f%%, Filtered = %.1f%%\n", potiValue, filtered[0]);
#if defined(USE_LMS)
    Serial.printf("LMS: Filtered = %.1f%%\n", filtered[1]);
#endif
  }

  delay(100); // 10 Hz
}

