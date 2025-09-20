#include <M5Atom.h>
#include <FastLED.h>
#include <Wire.h>

#define NUM_LEDS 10
#define LED_PIN 27
CRGB leds[NUM_LEDS];

#define POTI1_PIN 26  // NormalFreq (0.1-10 Hz)
#define POTI2_PIN 32  // Length (1-20)
#define POTI3_PIN 33  // MaxDecay (1s-24h)
#define POTI4_PIN 19  // Threshold (0-20%)

#define SLAVE_ADDRESS 0x08

void setup() {
  M5.begin(true, true, true);
  FastLED.addLeds<SK6812, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  Wire.begin(21, 22);
  Serial.begin(115200);
}

void loop() {
  float poti1 = analogRead(POTI1_PIN) / 4095.0f;
  float poti2 = analogRead(POTI2_PIN) / 4095.0f;
  float poti3 = analogRead(POTI3_PIN) / 4095.0f;
  float poti4 = analogRead(POTI4_PIN) / 4095.0f;

  float normalFreq = 0.1f + poti1 * 9.9f;
  int length = 1 + round(poti2 * 19);
  unsigned long maxDecay = 1000UL + round(poti3 * (86400000UL - 1000UL));
  float threshold = poti4 * 20.0f;

  // Sende Parameter für BME688-Kanäle (0-3) und GM (4)
  for (int channel = 0; channel < 5; channel++) {
    float freq = (channel == 4) ? normalFreq / 60.0f : normalFreq; // GM: 1/60 Hz
    sendI2C(1, channel, freq);
    sendI2C(2, channel, (float)length);
    sendI2C(3, channel, (float)maxDecay);
    sendI2C(4, channel, threshold);
    if (channel == 4) sendI2C(5, channel, 10.0f + poti4 * 290.0f); // Dead Time für GM
  }

  int hue = map(threshold * 10, 0, 200, 0, 255);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue, 255, 255);
  }
  FastLED.show();

  Serial.printf("Freq: %.4f Hz, Length: %d, Decay: %lu ms, Threshold: %.2f%%\n", normalFreq, length, maxDecay, threshold);
  delay(100);
}

void sendI2C(uint8_t cmd, uint8_t channel, float value) {
  Wire.beginTransmission(SLAVE_ADDRESS);
  Wire.write(cmd);
  Wire.write(channel);
  Wire.write((uint8_t*)&value, sizeof(float));
  Wire.endTransmission();
}