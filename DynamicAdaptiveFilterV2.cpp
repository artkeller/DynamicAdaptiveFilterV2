#include "DynamicAdaptiveFilterV2.h"
#include <algorithm>

DynamicAdaptiveFilterV2::DynamicAdaptiveFilterV2(const std::vector<FilterConfig>& configs) : _configs(configs) {
  _filters.resize(configs.size());
  // Initialisierung in begin() verschoben
}

void DynamicAdaptiveFilterV2::begin() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  for (size_t i = 0; i < _configs.size(); ++i) {
    if (!validateConfig(_configs[i])) {
      digitalWrite(LED_BUILTIN, LOW); // Fehler anzeigen
      while (true); // Stoppe bei ungültiger Konfiguration
    }
    initFilter(_filters[i], _configs[i]);
  }
  digitalWrite(LED_BUILTIN, LOW); // Initialisierung erfolgreich
}

bool DynamicAdaptiveFilterV2::validateConfig(const FilterConfig& config) {
  if (config.normalFreqHz <= 0 || config.thresholdPercent < 0 || config.deadTimeUs < 0 || config.maxDecayTimeMs < 1000) {
    return false;
  }
  if (config.type == FIR && (config.coeffs == nullptr || config.numCoeffs <= 0)) {
    return false;
  }
  if (config.type == EMA || config.type == SMA) {
    if (config.length < 1) return false;
  }
#if defined(USE_KALMAN)
  if (config.type == KALMAN && (config.Q <= 0 || config.R <= 0)) {
    return false;
  }
#endif
#if defined(USE_LMS)
  if (config.type == LMS && (config.mu <= 0 || config.length > MAX_FILTER_LENGTH || config.length < 1)) {
    return false;
  }
#endif
#if defined(USE_RLS)
  if (config.type == RLS && (config.lambda <= 0 || config.lambda > 1 || config.length > MAX_FILTER_LENGTH || config.length < 1)) {
    return false;
  }
#endif
  return true;
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
    if (config.mu <= 0) {
      const_cast<FilterConfig&>(config).mu = 0.01f; // Fallback
    }
    for (int i = 0; i < config.length; i++) {
      state.coeffs[i] = 0.0f;
      state.inputBuffer[i] = 0.0f;
    }
    state.bufferIndex = 0;
  }
#endif
#if defined(USE_RLS)
  else if (config.type == RLS) {
    if (config.lambda <= 0 || config.lambda > 1) {
      const_cast<FilterConfig&>(config).lambda = 0.9f; // Fallback
    }
    for (int i = 0; i < config.length; i++) {
      state.coeffs[i] = 0.0f;
      state.inputBuffer[i] = 0.0f;
    }
    state.bufferIndex = 0;
    state.P = 1.0f;
  }
#endif
}

float DynamicAdaptiveFilterV2::calculateMAD(float* data, int windowSize) {
  if (windowSize <= 0) return 0.0f;

  // Kopiere Daten
  float* temp = new float[windowSize];
  if (!temp) return 0.0f; // Speicherfehler
  for (int i = 0; i < windowSize; i++) {
    temp[i] = data[i];
  }

  // Berechne Median
  std::sort(temp, temp + windowSize);
  float median = (windowSize % 2 == 0) ?
    (temp[windowSize/2 - 1] + temp[windowSize/2]) / 2.0f :
    temp[windowSize/2];

  // Berechne absolute Abweichungen
  for (int i = 0; i < windowSize; i++) {
    temp[i] = abs(temp[i] - median);
  }

  // Berechne Median der Abweichungen
  std::sort(temp, temp + windowSize);
  float mad = (windowSize % 2 == 0) ?
    (temp[windowSize/2 - 1] + temp[windowSize/2]) / 2.0f :
    temp[windowSize/2];

  delete[] temp;
  return mad * 1.4826f; // Skalierungsfaktor
}

bool DynamicAdaptiveFilterV2::pushSensorData(const SensorData& data) {
  if (data.values.empty() || data.values.size() > _filters.size()) {
    return false;
  }

  bool success = true;
  for (size_t i = 0; i < data.values.size(); ++i) {
    FilterState& state = _filters[i];
    const FilterConfig& config = _configs[i];
    unsigned long currentTime = data.timestamp == 0 ? millis() : data.timestamp;
    unsigned long deltaT = currentTime > state.lastPushTime ? currentTime - state.lastPushTime : 0;
    state.lastPushTime = currentTime;

    float decayFactor = calculateDecayFactor(state, deltaT);
    float value = data.values[i];

    if (state.mode == VALUE_MODE && !isSignificantChange(state, value)) {
      continue;
    }

    if (state.mode == COUNT_MODE) {
      if (micros() - state.lastPushTime * 1000UL < state.deadTimeUs) {
        continue;
      }
      state.pulseCount++;
      continue;
    }

    if (config.type == EMA) {
      updateEMA(state, value, decayFactor);
    } else if (config.type == SMA || config.type == FIR) {
      pushToHistory(state, value);
      updateSMAorFIR(state, decayFactor);
    }
#if defined(USE_KALMAN)
    else if (config.type == KALMAN) {
      float K = state.P * (1.0f / (state.P + config.R));
      state.x = state.x + K * (value - state.x);
      state.P = (1.0f - K) * state.P + config.Q;
      state.filteredValue = state.x;
    }
#endif
#if defined(USE_LMS)
    else if (config.type == LMS) {
      // MAD-basierte Ausreißererkennung
      state.inputBuffer[state.bufferIndex] = value;
      float mad = calculateMAD(state.inputBuffer, config.length);
      float median = state.inputBuffer[config.length/2]; // Vereinfacht
      if (abs(value - median) > config.madThreshold * mad) {
        success = false; // Ausreißer ignorieren
        continue;
      }

      // Dynamische mu-Anpassung
      float dynamicMu = config.mu / (mad + 1e-6f);
      dynamicMu = constrain(dynamicMu, 0.001f, 0.1f);

      // LMS-Berechnung
      float output = 0.0f;
      for (int j = 0; j < config.length; j++) {
        int idx = (state.bufferIndex - j - 1 + config.length) % config.length;
        output += state.coeffs[j] * state.inputBuffer[idx];
      }
      float error = value - output;
      for (int j = 0; j < config.length; j++) {
        int idx = (state.bufferIndex - j - 1 + config.length) % config.length;
        state.coeffs[j] += dynamicMu * error * state.inputBuffer[idx];
      }
      state.filteredValue = output;
      state.inputBuffer[state.bufferIndex] = value;
      state.bufferIndex = (state.bufferIndex + 1) % config.length;
    }
#endif
#if defined(USE_RLS)
    else if (config.type == RLS) {
      // RLS-Implementierung (vereinfacht)
      state.inputBuffer[state.bufferIndex] = value;
      state.bufferIndex = (state.bufferIndex + 1) % config.length;
      state.filteredValue = value; // Platzhalter
    }
#endif
  }
  return success;
}

std::vector<float> DynamicAdaptiveFilterV2::getFilteredValues() const {
  std::vector<float> result(_filters.size());
  for (size_t i = 0; i < _filters.size(); ++i) {
    result[i] = _filters[i].filteredValue;
  }
  return result;
}

void DynamicAdaptiveFilterV2::updateNormalFreq(int channel, float normalFreqHz) {
  if (channel >= (int)_filters.size()) return;
  _filters[channel].normalFreqHz = max(0.01f, normalFreqHz);
  _filters[channel].expectedIntervalMs = static_cast<unsigned long>(1000.0f / _filters[channel].normalFreqHz);
}

void DynamicAdaptiveFilterV2::updateLength(int channel, int length) {
  if (channel >= (int)_filters.size()) return;
  if (_filters[channel].type == EMA || _filters[channel].type == SMA) {
    _filters[channel].baseAlpha = 2.0f / (max(1, length) + 1.0f);
    initSMA(_filters[channel], max(1, length));
  }
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

void DynamicAdaptiveFilterV2::onPulse(int channel) {
  if (channel >= (int)_filters.size()) return;
  _filters[channel].pulseCount++;
}

unsigned long DynamicAdaptiveFilterV2::getCPM(int channel) {
  if (channel >= (int)_filters.size()) return 0;
  unsigned long deltaT = millis() - _filters[channel].startTime;
  return _filters[channel].pulseCount * 60000UL / max(1UL, deltaT);
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

void DynamicAdaptiveFilterV2::initSMA(FilterState& state, int length) {
  state.baseCoeffs.resize(length, 1.0f / length);
  state.history.clear();
}

void DynamicAdaptiveFilterV2::initFIR(FilterState& state, const float* coeffs, int numCoeffs) {
  state.baseCoeffs.assign(coeffs, coeffs + numCoeffs);
  state.history.clear();
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
