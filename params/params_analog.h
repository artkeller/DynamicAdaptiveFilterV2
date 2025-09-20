#ifndef PARAMS_ANALOG_H
#define PARAMS_ANALOG_H

#include "DynamicAdaptiveFilterV2.h"

// FIR-Koeffizienten für Notch-Filter (50 Hz Brummen)
const float notch_50hz[] = {0.05f, 0.25f, 0.4f, 0.25f, 0.05f}; // Vereinfacht, 5 Taps, 50 Hz

// Makro zur Erstellung eines FilterConfig-Vektors für analogen Eingang
#define FILTER_ANALOG(pin, threshold, filter_type, length, coeffs, coeffs_len, freq_hz, decay_ms, warmup_ms) \
  std::vector<FilterConfig> filter_analog_##pin = { \
    {filter_type, length, coeffs, coeffs_len, freq_hz, decay_ms, warmup_ms, threshold, 0.0f, VALUE_MODE} \
  }

// Vordefinierte Filter für Szenarien

// Brummfilter (50/60 Hz, asymmetrische Zuleitungen)
const std::vector<FilterConfig> filter_analog_brum = {
  {FIR, 0, notch_50hz, 5, 100.0f, 10000, 200, 5.0f, 0.0f, VALUE_MODE} // 50 Hz Notch, ±0.165 V
};

// Spannungsteiler (Vbus/Vcc Rauschen)
const std::vector<FilterConfig> filter_analog_vdiv = {
  {EMA, 10, nullptr, 0, 10.0f, 3600000, 1000, 1.0f, 0.0f, VALUE_MODE} // ±0.033 V
};

// Potentiometer-Glättung
const std::vector<FilterConfig> filter_analog_poti = {
  {EMA, 10, nullptr, 0, 10.0f, 3600000, 1000, 2.0f, 0.0f, VALUE_MODE} // ±0.066 V
};

// Sensor-Rauschunterdrückung (z. B. LDR, NTC)
const std::vector<FilterConfig> filter_analog_sensor = {
  {SMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 2.0f, 0.0f, VALUE_MODE} // ±0.066 V, langsame Änderungen
};

// Schnelle Transienten (z. B. Stromsensor)
const std::vector<FilterConfig> filter_analog_transient = {
  {FIR, 0, notch_50hz, 5, 100.0f, 1000, 100, 5.0f, 0.0f, VALUE_MODE} // ±0.165 V, schnelle Reaktion
};

// Langsamer Drift (z. B. Batterieüberwachung)
const std::vector<FilterConfig> filter_analog_drift = {
  {EMA, 20, nullptr, 0, 0.1f, 86400000, 10000, 1.0f, 0.0f, VALUE_MODE} // ±0.033 V, sehr langsam
};

// Beispiel: Poti4 an GPIO19
FILTER_ANALOG(19, 2.0f, EMA, 10, nullptr, 0, 10.0f, 3600000, 1000);

#endif