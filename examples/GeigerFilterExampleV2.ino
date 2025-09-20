#include <Wire.h>
#include "DynamicAdaptiveFilter.h"
#include "params_GMCT.h"

#define SLAVE_ADDRESS 0x08
#define GM_PIN 2  // Interrupt-Pin für GM-Pulse (FALLING Edge)

// Wähle Rohrtyp
const GMCT_Params& gmType = GMCT_SBM20;  // Beispiel: SBM-20

// Initialisiere Filter mit GM-Parametern
DynamicAdaptiveFilter gmFilter(
  EMA, 
  gmType.recommendedLength,
  gmType.recommendedNormalFreqHz,
  86400000,  // MaxDecay: 24h
  60000,     // Warm-up: 60s
  gmType.recommendedThresholdPercent,
  COUNT_MODE,
  gmType.deadTimeUs
);

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
    float dose = filteredCpm * gmType.cpmToMicroSvPerHour;   // µSv/h
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
    case 5: gmFilter.updateDeadTime(value); break;
  }
}