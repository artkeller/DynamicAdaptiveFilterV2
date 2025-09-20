#include <Wire.h>
#include "DynamicAdaptiveFilter.h"
#include "filter_bessel.h"

#define SLAVE_ADDRESS 0x08
#define GM_PIN 2  // Interrupt-Pin für GM-Pulse (FALLING Edge)

// EMA für CPM-Glättung, 1/60 Hz (1 CPM-Intervall), 60s Warm-up, 5% Threshold, 100 µs Dead Time
DynamicAdaptiveFilter gmFilter(EMA, 10, 1.0f / 60.0f, 86400000, 60000, 5.0f, COUNT_MODE, 100.0f);

void setup() {
  Serial.begin(115200);
  pinMode(GM_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(GM_PIN), pulseISR, FALLING);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
}

void loop() {
  static unsigned long lastInterval = 0;
  unsigned long now = millis();
  if (now - lastInterval >= 60000) {  // Alle 60 s
    unsigned long cpm = gmFilter.getCPM();
    float filteredCpm = gmFilter.getFilteredValue() * 60.0f;  // CPS zu CPM
    float dose = filteredCpm / 220.0f;  // µSv/h für SBM-20
    Serial.printf("CPM: %lu, Filtered: %.1f, Dose: %.2f µSv/h\n", cpm, filteredCpm, dose);
    lastInterval = now;
  }
}

void pulseISR() {
  gmFilter.onPulse();
}

void receiveEvent(int numBytes) {
  if (numBytes < 1 + sizeof(float)) return;
  uint8_t cmd = Wire.read();
  uint8_t buf[sizeof(float)];
  for (int i = 0; i < sizeof(float); i++) {
    buf[i] = Wire.read();
  }
  float value = *(float*)buf;

  switch (cmd) {
    case 1: gmFilter.updateNormalFreq(value); break;
    case 2: gmFilter.updateLength((int)value); break;
    case 3: gmFilter.updateMaxDecayTime((unsigned long)value); break;
    case 4: gmFilter.updateThreshold(value); break;
    case 5: gmFilter.updateDeadTime(value); break;  // Neu: Dead Time
  }
}