#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include "DynamicAdaptiveFilterV2.h"
#include "FIR_coefficients" // chebyshev filter für FIR-Position, bessel filter für FIR-Geschwindigkeit und Richtung

#define SLAVE_ADDRESS 0x08
#define GPS_RX 16
#define GPS_TX 17

HardwareSerial gpsSerial(1);
TinyGPSPlus gps;

const float pos_coeffs[] = {chebyshev_lowpass_order2, 5};  // FIR für Position (5 Taps)
const float speed_coeffs[] = {bessel_lowpass_order2, 5};   // FIR für Speed/Heading

// Filter-Konfigs für GPS (6 Kanäle): Lat, Lon, Alt, Speed, Heading, Sats
std::vector<FilterConfig> gpsConfigs = {
  {FIR, 0, chebyshev_lowpass_order2, 5, 1.0f, 86400000, 10000, 1.0f, 0.0f, VALUE_MODE}, // Lat: FIR, 1 Hz, 1% Threshold
  {FIR, 0, chebyshev_lowpass_order2, 5, 1.0f, 86400000, 10000, 1.0f, 0.0f, VALUE_MODE}, // Lon: FIR
  {EMA, 10, nullptr, 0, 0.5f, 86400000, 10000, 2.0f, 0.0f, VALUE_MODE},                // Alt: EMA, Länge 10
  {FIR, 0, bessel_lowpass_order2, 5, 5.0f, 3600000, 10000, 5.0f, 0.0f, VALUE_MODE},    // Speed: FIR, 5 Hz, 5% Threshold
  {FIR, 0, bessel_lowpass_order2, 5, 5.0f, 3600000, 10000, 5.0f, 0.0f, VALUE_MODE},    // Heading: FIR
  {SMA, 5, nullptr, 0, 1.0f, 86400000, 10000, 0.0f, 0.0f, VALUE_MODE}                  // Sats: SMA, kein Threshold
};

DynamicAdaptiveFilterV2 gpsFilter(gpsConfigs);

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
  Serial.println("GPS M10 Filter gestartet...");
}

void loop() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid() && gps.altitude.isValid() && gps.speed.isValid()) {
        SensorData data;
        data.values = {
          gps.location.lat(),      // Lat (Grad)
          gps.location.lng(),      // Lon (Grad)
          gps.altitude.meters(),   // Höhe (m)
          gps.speed.kmph(),        // Speed (km/h)
          gps.course.deg(),        // Heading (Grad)
          (float)gps.satellites.value()  // Sats
        };
        data.timestamp = millis();
        data.sensorId = "NEO-M10";
        gpsFilter.pushSensorData(data);
      }
    }
  }

  // Ausgabe alle 1 s (nach Warm-up)
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 1000) {
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
