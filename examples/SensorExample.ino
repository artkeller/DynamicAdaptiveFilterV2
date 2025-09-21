#include "DynamicAdaptiveFilterV2.h"
#include "params/params_GMCT.h"

// Optional: Kalman für GPS
#define USE_KALMAN

FilterConfig tempConfig = {
  EMA, 10, nullptr, 0, 1.0f, 5000, 1000, 5.0f, 0.0f, VALUE_MODE
};

#if defined(USE_KALMAN)
FilterConfig gpsConfig = {
  KALMAN, 0, nullptr, 0, 10.0f, 10000, 1000, 5.0f, 0.0f, VALUE_MODE, 0.01f, 0.1f, 0.0f
};
DynamicAdaptiveFilterV2 filter({tempConfig, filter_sbm20[0], gpsConfig});
#else
DynamicAdaptiveFilterV2 filter({tempConfig, filter_sbm20[0]});
#endif

void IRAM_ATTR onGeigerPulse() {
  filter.onPulse(1); // Kanal 1 (Geiger)
}

void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT); // Temperatur
  pinMode(4, INPUT_PULLUP); // Geiger-Zähler
  attachInterrupt(digitalPinToInterrupt(4), onGeigerPulse, FALLING);
  Serial.println("Filter gestartet: Temperatur (EMA), Geiger-Zähler (COUNT_MODE)");
}

void loop() {
  float temp = analogRead(A0) * (3.3f / 4095.0f) * 100.0f; // Spannung -> °C
  float speed = 50.0f + random(-500, 500) / 100.0f; // Simulierte GPS-Daten
  SensorData data;
#if defined(USE_KALMAN)
  data = {{temp, 0.0f, speed}, {}, millis(), "TEMP_GM_GPS"};
#else
  data = {{temp, 0.0f}, {}, millis(), "TEMP_GM"};
#endif
  if (filter.pushSensorData(data)) {
    auto values = filter.getFilteredValues();
    Serial.printf("Temperatur: %.1f°C (Gefiltert: %.1f°C)\n", temp, values[0]);
  }

  unsigned long cpm = filter.getCPM(1);
  if (cpm > 0) {
    auto values = filter.getFilteredValues();
    Serial.printf("Geiger-Zähler: %lu CPM (Gefilterte CPS: %.1f)\n", cpm, values[1]);
  }

#if defined(USE_KALMAN)
  if (filter.pushSensorData(data)) {
    auto values = filter.getFilteredValues();
    Serial.printf("GPS-Geschwindigkeit: %.1f km/h (Gefiltert: %.1f km/h)\n", speed, values[2]);
  }
#endif

  delay(1000); // 1 Hz
}
