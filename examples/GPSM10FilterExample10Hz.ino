#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include "DynamicAdaptiveFilterV2.h"
#include "filter_chebyshev.h"
#include "filter_bessel.h"

#define SLAVE_ADDRESS 0x08
#define GPS_RX 16
#define GPS_TX 17

HardwareSerial gpsSerial(1);
TinyGPSPlus gps;

const float pos_coeffs[] = {chebyshev_lowpass_order2, 5};  // FIR für Position
const float speed_coeffs[] = {bessel_lowpass_order2, 5};   // FIR für Speed/Heading

// Filter-Konfigs für 10 Hz GPS: Lat, Lon, Alt, Speed, Heading, Sats
std::vector<FilterConfig> gpsConfigs = {
  {FIR, 0, chebyshev_lowpass_order2, 5, 2.0f, 86400000, 10000, 1.0f, 0.0f, VALUE_MODE}, // Lat
  {FIR, 0, chebyshev_lowpass_order2, 5, 2.0f, 86400000, 10000, 1.0f, 0.0f, VALUE_MODE}, // Lon
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 10000, 2.0f, 0.0f, VALUE_MODE},                // Alt
  {FIR, 0, bessel_lowpass_order2, 5, 10.0f, 3600000, 10000, 5.0f, 0.0f, VALUE_MODE},   // Speed
  {FIR, 0, bessel_lowpass_order2, 5, 10.0f, 3600000, 10000, 5.0f, 0.0f, VALUE_MODE},   // Heading
  {SMA, 5, nullptr, 0, 2.0f, 86400000, 10000, 0.0f, 0.0f, VALUE_MODE}                  // Sats
};

DynamicAdaptiveFilterV2 gpsFilter(gpsConfigs);

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(115200, SERIAL_8N1, GPS_RX, GPS_TX); // 115200 Baud
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);

  // Konfiguriere 10 Hz Update-Rate (UBX-CFG-RATE)
  uint8_t ubxCfgRate[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x64, 0x00, 0x01, 0x00, 0x01, 0x00, 0x7A, 0x12};
  gpsSerial.write(ubxCfgRate, sizeof(ubxCfgRate)); // 100 ms (10 Hz)
  Serial.println("NEO-M10 konfiguriert für 10 Hz, 115200 Baud...");
}

void loop() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid() && gps.altitude.isValid() && gps.speed.isValid()) {
        SensorData data;
        data.values = {
          (float)gps.location.lat(),      // Lat (Grad)
          (float)gps.location.lng(),      // Lon (Grad)
          gps.altitude.meters(),          // Höhe (m)
          gps.speed.kmph(),               // Speed (km/h)
          gps.course.deg(),               // Heading (Grad)
          (float)gps.satellites.value()   // Sats
        };
        data.timestamp = millis();
        data.sensorId = "NEO-M10";
        gpsFilter.pushSensorData(data);
      }
    }
  }

  // Ausgabe alle 200 ms (5 Hz, um Serial nicht zu überlasten)
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 200) {
    auto filtered = gpsFilter.getFilteredValues();
    Serial.printf("Lat: %.6f, Lon: %.6f, Alt: %.1f m, Speed: %.1f km/h, Heading: %.1f°, Sats: %.0f\n",
                  filtered[0], filtered[1], filtered[2], filtered[3], filtered[4], filtered[5]);
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
    case 1: gpsFilter.updateNormalFreq(channel, value); break;
    case 2: gpsFilter.updateLength(channel, (int)value); break;
    case 3: gpsFilter.updateMaxDecayTime(channel, (unsigned long)value); break;
    case 4: gpsFilter.updateThreshold(channel, value); break;
  }
}