#include <Wire.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <Adafruit_BME680.h>
#include <Adafruit_SHT31.h>
#include "DynamicAdaptiveFilterV2.h"
#include "params_sensors.h"

#define SLAVE_ADDRESS 0x08
#define GPS_RX 16
#define GPS_TX 17

HardwareSerial gpsSerial(1);
TinyGPSPlus gps;
Adafruit_BME680 bme;
Adafruit_SHT31 sht = Adafruit_SHT31();

// Kombinierte Filterkonfiguration
std::vector<FilterConfig> configs = {
  filter_bme688[0], // BME688: Temperatur
  filter_bme688[1], // Feuchte
  filter_bme688[2], // Druck
  filter_bme688[3], // Gas
  filter_sht45[0],  // SHT45: Temperatur
  filter_sht45[1],  // Feuchte
  filter_gps_neo_m10[0], // Lat
  filter_gps_neo_m10[1], // Lon
  filter_gps_neo_m10[2], // Höhe
  filter_gps_neo_m10[3], // Geschwindigkeit
  filter_gps_neo_m10[4], // Kurs
  filter_gps_neo_m10[5]  // Satellitenanzahl
};

DynamicAdaptiveFilterV2 filter(configs);

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(115200, SERIAL_8N1, GPS_RX, GPS_TX);
  uint8_t ubxCfgRate[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x64, 0x00, 0x01, 0x00, 0x01, 0x00, 0x7A, 0x12};
  gpsSerial.write(ubxCfgRate, sizeof(ubxCfgRate)); // 10 Hz
  Wire.begin();
  if (!bme.begin()) {
    Serial.println("BME688 nicht gefunden!");
    while (true);
  }
  if (!sht.begin(0x44)) {
    Serial.println("SHT45 nicht gefunden!");
    while (true);
  }
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
  Serial.println("Multi-Sensor-Filter gestartet...");
}

void loop() {
  // BME688 und SHT45: Alle 30 Minuten (langsame Änderungen)
  static unsigned long lastEnv = 0;
  if (millis() - lastEnv >= 1800000) {
    if (bme.performReading()) {
      SensorData bmeData;
      bmeData.values = {bme.temperature, bme.humidity, bme.pressure / 100.0f, bme.gas_resistance / 1000.0f};
      bmeData.timestamp = millis();
      bmeData.sensorId = "BME688";
      filter.pushSensorData(bmeData);
    }
    float temp, hum;
    if (sht.readBoth(&temp, &hum)) {
      SensorData shtData;
      shtData.values = {temp, hum};
      shtData.timestamp = millis();
      shtData.sensorId = "SHT45";
      filter.pushSensorData(shtData);
    }
    lastEnv = millis();
  }

  // GPS: 10 Hz
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid() && gps.altitude.isValid() && gps.speed.isValid()) {
        SensorData gpsData;
        gpsData.values = {
          (float)gps.location.lat(),
          (float)gps.location.lng(),
          gps.altitude.meters(),
          gps.speed.kmph(),
          gps.course.deg(),
          (float)gps.satellites.value()
        };
        gpsData.timestamp = millis();
        gpsData.sensorId = "NEO-M10";
        filter.pushSensorData(gpsData);
      }
    }
  }

  // Ausgabe alle 200 ms (5 Hz)
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 200) {
    auto filtered = filter.getFilteredValues();
    Serial.printf("BME688: T=%.1f°C, H=%.1f%%, P=%.1fhPa, G=%.1fkOhm | "
                  "SHT45: T=%.1f°C, H=%.1f%% | "
                  "GPS: Lat=%.6f, Lon=%.6f, Alt=%.1fm, Spd=%.1fkm/h, Hdg=%.1f°, Sats=%.0f\n",
                  filtered[0], filtered[1], filtered[2], filtered[3],
                  filtered[4], filtered[5],
                  filtered[6], filtered[7], filtered[8], filtered[9], filtered[10], filtered[11]);
    lastPrint = millis();
  }
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
  }
}