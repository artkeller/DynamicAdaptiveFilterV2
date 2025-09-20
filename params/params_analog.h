#ifndef PARAMS_ANALOG_H
#define PARAMS_ANALOG_H

#include "DynamicAdaptiveFilterV2.h"

// Makro zur Erstellung eines FilterConfig-Vektors für analogen Eingang
#define FILTER_ANALOG(pin, threshold, filter_type, length, freq_hz, decay_ms, warmup_ms) \
  std::vector<FilterConfig> filter_analog_##pin = { \
    {filter_type, length, nullptr, 0, freq_hz, decay_ms, warmup_ms, threshold, 0.0f, VALUE_MODE} \
  }

#endif