#ifndef PARAMS_ANALOG_H
#define PARAMS_ANALOG_H

#include <vector>
#include "DynamicAdaptiveFilterV2.h"

// _TRAILER-Makro für FilterConfig-Initialisierung
#if defined(USE_LMS) || defined(USE_RLS)
  #define _TRAILER ,0.01f,3.0f  // mu=0.01, madThreshold=3.0
#elif defined(USE_KALMAN)
  #define _TRAILER ,0.01f,0.01f,0.01f,3.0f  // Q=0.01, R=0.01, initialState=0.01, madThreshold=3.0
#else
  #define _TRAILER ,3.0f  // madThreshold=3.0
#endif

// Makro für FilterConfig-Vektor
#define FILTER_ANALOG(pin, threshold, filter_type, length, coeffs, coeffs_len, freq_hz, decay_ms, warmup_ms) \
  std::vector<FilterConfig> filter_analog_##pin = { \
    {filter_type, length, coeffs, coeffs_len, freq_hz, decay_ms, warmup_ms, threshold, 0.0f, VALUE_MODE _TRAILER} \
  }

// Vordefinierte Filter für Szenarien
const std::vector<FilterConfig> filter_analog_brum = {
  {FIR, 5, notch_50hz, 5, 100.0f, 10000, 200, 5.0f, 0.0f, VALUE_MODE} // 50 Hz Notch
};

const std::vector<FilterConfig> filter_analog_vdiv = {
  {EMA, 10, nullptr, 0, 10.0f, 3600000, 1000, 1.0f, 0.0f, VALUE_MODE}
};

const std::vector<FilterConfig> filter_analog_poti = {
  {EMA, 10, nullptr, 0, 10.0f, 3600000, 1000, 2.0f, 0.0f, VALUE_MODE}
};

const std::vector<FilterConfig> filter_analog_sensor = {
  {SMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 2.0f, 0.0f, VALUE_MODE}
};

const std::vector<FilterConfig> filter_analog_transient = {
  {FIR, 5, notch_50hz, 5, 100.0f, 1000, 100, 5.0f, 0.0f, VALUE_MODE}
};

const std::vector<FilterConfig> filter_analog_drift = {
  {EMA, 20, nullptr, 0, 0.1f, 86400000, 10000, 1.0f, 0.0f, VALUE_MODE}
};

#endif


