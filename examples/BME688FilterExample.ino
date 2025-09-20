#include <Wire.h>
#include "DynamicAdaptiveFilterV2.h"
#include "params/params_GMCT.h"
#include <Adafruit_BME680.h>

#define SLAVE_ADDRESS 0x08
Adafruit_BME680 bme;

const float bessel_coeffs[] = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f}; // Beispiel FIR

// Filter-Konfigurationen für BME688 (4 Kanäle) + GM-Zähler
std::vector<FilterConfig> configs = {
  {EMA, 15, nullptr, 0, 0.5f, 86400000, 5000, 5.0f, 0.0f, VALUE_MODE}, // Temperatur
  {SMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 5.0f, 0.0f, VALUE_MODE}, // Feuchte
  {FIR, 0, bessel_coeffs, 5, 0.5f, 86400000, 5000, 3.0f, 0.0f, VALUE_MODE}, // Druck
  {FIR, 0, bessel_coeffs, 5, 1.0f, 86400000, 5000, 10.0f, 0.0f, VALUE_MODE}, // Gas
  {EMA, GMCT_SBM20.recommendedLength, nullptr, 0, GMCT_SBM20.recommendedNormalFreqHz, 
   86400000, 60000, GMCT_SBM20.recommendedThresholdPercent, GMCT_SBM20.deadTimeUs, COUNT_MODE} // GM-Zähler
};

DynamicAdaptiveFilterV2 filter(configs);
#define GM_PIN 2

void setup() {
  Serial.begin(115200);
  if (!bme.begin()) {
    Serial.println("BME688 nicht gefunden!");
    while (true);
  }
  pinMode(GM_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(GM_PIN), pulseISR, FALLING);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
}

void loop() {
  static unsigned long lastMeasurement = 0;
  const unsigned long interval = 1800000; // 30 Minuten
  if (millis() - lastMeasurement >= interval) {
    // BME688 Daten lesen
    if (bme.performReading()) {
      SensorData data;
      data.values = {bme.temperature, bme.humidity, bme.pressure / 100.0f, bme.gas_resistance / 1000.0f};
      data.timestamp = millis();
      data.sensorId = "BME688";
      filter.pushSensorData(data);
    }
    // GM CPM holen
    unsigned long cpm = filter.getCPM(4); // Kanal 4 = GM
    auto filtered = filter.getFilteredValues();
    float dose = filtered[4] * 60.0f * GMCT_SBM20.cpmToMicroSvPerHour;
    Serial.printf("Temp: %.1f °C, Feuchte: %.1f %%, Druck: %.1f hPa, Gas: %.1f kOhm, CPM: %lu, Dose: %.2f µSv/h\n",
                  filtered[0], filtered[1], filtered[2], filtered[3], cpm, dose);
    lastMeasurement = millis();
  }
}

void pulseISR() {
  filter.onPulse(4); // Kanal 4 = GM
}

void receiveEvent(int numBytes) {
  if (numBytes < 2 + sizeof(float)) return;
  uint8_t cmd = Wire.read();
  uint8_t channel = Wire.read();
  uint8_t buf[sizeof(float)];
  for (int i = 0; i < sizeof(float); i++) {
    buf[i] = Wire.read();
  }
  float value = *(float*)buf;

  switch (cmd) {
    case 1: filter.updateNormalFreq(channel, value); break;
    case 2: filter.updateLength(channel, (int)value); break;
    case 3: filter.updateMaxDecayTime(channel, (unsigned long)value); break;
    case 4: filter.updateThreshold(channel, value); break;
    case 5: filter.updateDeadTime(channel, value); break;
  }

}
