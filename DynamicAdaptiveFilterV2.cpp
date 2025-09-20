#include "DynamicAdaptiveFilterV2.h"

DynamicAdaptiveFilterV2::DynamicAdaptiveFilterV2(const std::vector<FilterConfig>& configs) : _configs(configs) {
  _filters.resize(configs.size());
  for (size_t i = 0; i < configs.size(); ++i) {
    initFilter(_filters[i], configs[i]);
  }
}

void DynamicAdaptiveFilterV2::initFilter(FilterState& state, const FilterConfig& config) {
  state.type = config.type;
  state.normalFreqHz = max(0.01f, config.normalFreqHz);
  state.expectedIntervalMs = static_cast<unsigned long>(1000.0f / state.normalFreqHz);
  state.maxDecayTimeMs = max(1000UL, config.maxDecayTimeMs);
  state.warmUpTimeMs = config.mode == COUNT_MODE ? max(config.warmUpTimeMs, 60000UL) : config.warmUpTimeMs;
  state.thresholdPercent = max(0.0f, config.thresholdPercent);
  state.deadTimeUs = max(10.0f, config.deadTimeUs);
  state.mode = config.mode;
  state.startTime = millis();
  state.lastPushTime = 0;
  state.filteredValue = 0.0f;
  state.pulseCount = 0;

  if (config.type == EMA) {
    state.baseAlpha = 2.0f / (max(1, config.length) + 1.0f);
  } else if (config.type == SMA) {
    initSMA(state, max(1, config.length));
  } else if (config.type == FIR) {
    initFIR(state, config.coeffs, config.numCoeffs);
  }
#if defined(USE_KALMAN)
  else if (config.type == KALMAN) {
    state.P = 1.0f;
    state.x = config.initialState;
  }
#endif
#if defined(USE_LMS)
  else if (config.type == LMS) {
    for (int i = 0; i < config.length; i++) {
      state.coeffs[i] = 0.0f;
      state.inputBuffer[i] = 0.0f;
    }
    state.bufferIndex = 0;
  }
#endif
#if defined(USE_RLS)
  else if (config.type == RLS) {
    for (int i = 0; i < config.length; i++) {
      state.coeffs[i] = 0.0f;
      state.inputBuffer[i] = 0.0f;
    }
    state.bufferIndex = 0;
    state.P = 1.0f;
  }
#endif
}

void DynamicAdaptiveFilterV2::initSMA(FilterState& state, int length) {
  state.baseCoeffs.resize(length, 1.0f / length);
  state.history.clear();
  state.history.reserve(length);
}

void DynamicAdaptiveFilterV2::initFIR(FilterState& state, const float* coeffs, int numCoeffs) {
  state.baseCoeffs.assign(coeffs, coeffs + numCoeffs);
  state.history.clear();
  state.history.reserve(numCoeffs);
  float sum = 0.0f;
  for (int i = 0; i < numCoeffs; ++i) sum += state.baseCoeffs[i];
  if (abs(sum - 1.0f) > 0.01f) {
    Serial.println("Warning: FIR coeffs sum not ~1");
  }
}

void DynamicAdaptiveFilterV2::pushSensorData(const SensorData& data) {
  if (data.values.size() != _filters.size()) {
    Serial.println("Error: Mismatch in number of values and filters");
    return;
  }
  _sensorId = data.sensorId;
  unsigned long currentTime = data.timestamp;
  for (size_t i = 0; i < _filters.size(); ++i) {
    FilterState& state = _filters[i];
    const FilterConfig& config = _configs[i];
    if (state.mode == COUNT_MODE && data.values[i] == 1.0f) {
      continue; // Pulse werden via onPulse() verarbeitet
    }
    if ((currentTime - state.startTime) < state.warmUpTimeMs) {
      continue;
    }
    if (state.lastPushTime == 0 || !isSignificantChange(state, data.values[i])) {
      if (state.lastPushTime == 0) {
        state.filteredValue = data.values[i];
        if (state.type == SMA || state.type == FIR) {
          initializeHistory(state, data.values[i]);
        }
#if defined(USE_KALMAN)
        else if (state.type == KALMAN) {
          state.x = data.values[i];
          state.filteredValue = state.x;
        }
#endif
#if defined(USE_LMS)
        else if (state.type == LMS) {
          for (int j = 0; j < config.length; j++) {
            state.inputBuffer[j] = data.values[i];
          }
          state.filteredValue = data.values[i];
        }
#endif
#if defined(USE_RLS)
        else if (state.type == RLS) {
          for (int j = 0; j < config.length; j++) {
            state.inputBuffer[j] = data.values[i];
          }
          state.filteredValue = data.values[i];
        }
#endif
        state.lastPushTime = currentTime;
      }
      continue;
    }
    unsigned long deltaT = currentTime - state.lastPushTime;
    if (deltaT < state.expectedIntervalMs / 2) {
      continue;
    }
    float decayFactor = calculateDecayFactor(state, deltaT);
    if (state.type == EMA) {
      updateEMA(state, data.values[i], decayFactor);
    } else if (state.type == SMA || state.type == FIR) {
      pushToHistory(state, data.values[i]);
      updateSMAorFIR(state, decayFactor);
    }
#if defined(USE_KALMAN)
    else if (state.type == KALMAN) {
      // Vorhersage
      float x_pred = state.x; // Einfaches Modell (konstant)
      float P_pred = state.P + config.Q;
      // Korrektur
      float K = P_pred / (P_pred + config.R);
      state.x = x_pred + K * (data.values[i] - x_pred);
      state.P = (1 - K) * P_pred;
      // Adaptiv: R anpassen
      float error = data.values[i] - state.x;
      config.R = 0.99f * config.R + 0.01f * error * error;
      state.filteredValue = state.x;
    }
#endif
#if defined(USE_LMS)
    else if (state.type == LMS) {
      state.inputBuffer[state.bufferIndex] = data.values[i];
      state.bufferIndex = (state.bufferIndex + 1) % config.length;
      float output = 0.0f;
      for (int j = 0; j < config.length; j++) {
        output += state.coeffs[j] * state.inputBuffer[(state.bufferIndex + j) % config.length];
      }
      float error = data.values[i] - output; // Selbst-adaptive Annahme
      for (int j = 0; j < config.length; j++) {
        state.coeffs[j] += config.mu * error * state.inputBuffer[(state.bufferIndex + j) % config.length];
      }
      state.filteredValue = output;
    }
#endif
#if defined(USE_RLS)
    else if (state.type == RLS) {
      state.inputBuffer[state.bufferIndex] = data.values[i];
      state.bufferIndex = (state.bufferIndex + 1) % config.length;
      float output = 0.0f;
      for (int j = 0; j < config.length; j++) {
        output += state.coeffs[j] * state.inputBuffer[(state.bufferIndex + j) % config.length];
      }
      float error = data.values[i] - output;
      float gain = state.P / (config.lambda + state.P);
      for (int j = 0; j < config.length; j++) {
        state.coeffs[j] += gain * error * state.inputBuffer[(state.bufferIndex + j) % config.length];
      }
      state.P = (state.P - gain * state.P) / config.lambda;
      state.filteredValue = output;
    }
#endif
    state.lastPushTime = currentTime;
  }
}

std::vector<float> DynamicAdaptiveFilterV2::getFilteredValues() const {
  std::vector<float> result(_filters.size());
  for (size_t i = 0; i < _filters.size(); ++i) {
    result[i] = _filters[i].filteredValue;
  }
  return result;
}

void DynamicAdaptiveFilterV2::onPulse(int channel) {
  if (channel >= (int)_filters.size() || _filters[channel].mode != COUNT_MODE) return;
  static unsigned long lastPulse[4] = {0}; // Unterstützt bis zu 4 Kanäle
  unsigned long now = micros();
  if (now - lastPulse[channel] > state.deadTimeUs) {
    _filters[channel].pulseCount++;
    lastPulse[channel] = now;
  }
}

unsigned long DynamicAdaptiveFilterV2::getCPM(int channel) {
  if (channel >= (int)_filters.size() || _filters[channel].mode != COUNT_MODE) return 0;
  FilterState& state = _filters[channel];
  static unsigned long lastInterval[4] = {0};
  unsigned long now = millis();
  if (now - lastInterval[channel] < 60000) return 0;
  unsigned long cpm = state.pulseCount * (60000UL / (now - lastInterval[channel]));
  state.pulseCount = 0;
  lastInterval[channel] = now;
  float cps = cpm / 60.0f;
  float correctedCps = cps / (1.0f - cps * state.deadTimeUs * 1e-6f);
  pushSensorData({{correctedCps}, now, _sensorId}); // Push korrigierte CPS
  return round(cpm);
}

void DynamicAdaptiveFilterV2::updateNormalFreq(int channel, float normalFreqHz) {
  if (channel >= (int)_filters.size()) return;
  _filters[channel].normalFreqHz = max(0.01f, normalFreqHz);
  _filters[channel].expectedIntervalMs = static_cast<unsigned long>(1000.0f / _filters[channel].normalFreqHz);
}

void DynamicAdaptiveFilterV2::updateLength(int channel, int length) {
  if (channel >= (int)_filters.size()) return;
  FilterState& state = _filters[channel];
  if (state.type == EMA) {
    state.baseAlpha = 2.0f / (max(1, length) + 1.0f);
  } else if (state.type == SMA) {
    initSMA(state, max(1, length));
  }
#ifdef USE_LMS
  else if (state.type == LMS) {
    for (int i = 0; i < length; i++) {
      state.coeffs[i] = 0.0f;
      state.inputBuffer[i] = 0.0f;
    }
    state.bufferIndex = 0;
  }
#endif
#ifdef USE_RLS
  else if (state.type == RLS) {
    for (int i = 0; i < length; i++) {
      state.coeffs[i] = 0.0f;
      state.inputBuffer[i] = 0.0f;
    }
    state.bufferIndex = 0;
    state.P = 1.0f;
  }
#endif
}

void DynamicAdaptiveFilterV2::updateFIRCoeffs(int channel, const float* coeffs, int numCoeffs) {
  if (channel >= (int)_filters.size() || _filters[channel].type != FIR) return;
  initFIR(_filters[channel], coeffs, numCoeffs);
}

void DynamicAdaptiveFilterV2::updateMaxDecayTime(int channel, unsigned long maxDecayTimeMs) {
  if (channel >= (int)_filters.size()) return;
  _filters[channel].maxDecayTimeMs = max(1000UL, maxDecayTimeMs);
}

void DynamicAdaptiveFilterV2::updateThreshold(int channel, float thresholdPercent) {
  if (channel >= (int)_filters.size()) return;
  _filters[channel].thresholdPercent = max(0.0f, thresholdPercent);
}

void DynamicAdaptiveFilterV2::updateDeadTime(int channel, float deadTimeUs) {
  if (channel >= (int)_filters.size()) return;
  _filters[channel].deadTimeUs = max(10.0f, deadTimeUs);
}

void DynamicAdaptiveFilterV2::updateMode(int channel, FilterMode mode) {
  if (channel >= (int)_filters.size()) return;
  _filters[channel].mode = mode;
  if (mode == COUNT_MODE) _filters[channel].warmUpTimeMs = max(_filters[channel].warmUpTimeMs, 60000UL);
}

float DynamicAdaptiveFilterV2::calculateDecayFactor(const FilterState& state, unsigned long deltaT) const {
  if (deltaT <= state.expectedIntervalMs) {
    return 1.0f;
  } else if (deltaT >= state.maxDecayTimeMs) {
    return 0.0f;
  } else {
    return 1.0f - static_cast<float>(deltaT - state.expectedIntervalMs) / static_cast<float>(state.maxDecayTimeMs - state.expectedIntervalMs);
  }
}

void DynamicAdaptiveFilterV2::updateEMA(FilterState& state, float value, float decayFactor) {
  float effectiveAlpha = 1.0f - decayFactor * (1.0f - state.baseAlpha);
  state.filteredValue = effectiveAlpha * value + (1.0f - effectiveAlpha) * state.filteredValue;
}

void DynamicAdaptiveFilterV2::updateSMAorFIR(FilterState& state, float decayFactor) {
  int num = state.baseCoeffs.size();
  int histSize = state.history.size();
  state.filteredValue = 0.0f;
  float sumScaled = 0.0f;
  if (histSize > 0) {
    state.filteredValue += state.baseCoeffs[0] * state.history.back();
    sumScaled += state.baseCoeffs[0];
  }
  for (int i = 1; i < num; ++i) {
    if (i >= histSize) break;
    float pastCoeff = state.baseCoeffs[i] * decayFactor;
    state.filteredValue += pastCoeff * state.history[histSize - 1 - i];
    sumScaled += pastCoeff;
  }
  if (sumScaled < 1.0f && histSize > 0) {
    state.filteredValue += (1.0f - sumScaled) * state.history.back();
  }
}

void DynamicAdaptiveFilterV2::pushToHistory(FilterState& state, float value) {
  state.history.push_back(value);
  if (state.history.size() > state.baseCoeffs.size()) {
    state.history.erase(state.history.begin());
  }
}

void DynamicAdaptiveFilterV2::initializeHistory(FilterState& state, float value) {
  int num = state.baseCoeffs.size();
  state.history.assign(num, value);
  updateSMAorFIR(state, 1.0f);
}

bool DynamicAdaptiveFilterV2::isSignificantChange(const FilterState& state, float value) const {
  if (state.thresholdPercent == 0.0f) return true;
  if (abs(state.filteredValue) < 1e-6) return true;
  float change = abs(value - state.filteredValue) / abs(state.filteredValue);
  return change * 100.0f >= state.thresholdPercent;
}
