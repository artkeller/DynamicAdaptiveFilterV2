#ifndef DYNAMIC_ADAPTIVE_FILTER_V2_H
#define DYNAMIC_ADAPTIVE_FILTER_V2_H

#include <Arduino.h>
#include <vector>
#include <string>

// Filtertypen
enum FilterType {
  EMA,  // Exponential Moving Average
  SMA,  // Simple Moving Average
  FIR   // Finite Impulse Response
};

// Filtermodi
enum FilterMode {
  VALUE_MODE,  // Für kontinuierliche Werte (z.B. ADC)
  COUNT_MODE   // Für Zählimpulse (z.B. GM)
};

// Konfigurationsstruktur für jeden Messkanal
struct FilterConfig {
  FilterType type;
  int length;                    // Für EMA/SMA: Fensterlänge; FIR: Ignoriert
  const float* coeffs;           // Für FIR: Koeffizienten
  int numCoeffs;                // Für FIR: Anzahl Koeffizienten
  float normalFreqHz;           // Normale Frequenz (Hz)
  unsigned long maxDecayTimeMs; // Max. Decay-Zeit (ms)
  unsigned long warmUpTimeMs;   // Warm-up-Zeit (ms)
  float thresholdPercent;       // Schmitt-Trigger-Threshold (%)
  float deadTimeUs;             // Dead Time (µs, für COUNT_MODE)
  FilterMode mode;              // VALUE_MODE oder COUNT_MODE
};

// Sensor-Datenstruktur
struct SensorData {
  std::vector<float> values;  // Messwerte (z.B. [Temp, Feuchte, Druck])
  unsigned long timestamp;    // Zeitstempel (millis())
  String sensorId;           // Sensor-ID (z.B. "BME688")
};

class DynamicAdaptiveFilterV2 {
public:
  // Konstruktor: Akzeptiert Vektor von FilterConfigs für jeden Kanal
  DynamicAdaptiveFilterV2(const std::vector<FilterConfig>& configs);

  // Methoden für Sensor-Daten
  void pushSensorData(const SensorData& data);
  std::vector<float> getFilteredValues() const;

  // Dynamische Update-Methoden (pro Kanal)
  void updateNormalFreq(int channel, float normalFreqHz);
  void updateLength(int channel, int length);
  void updateFIRCoeffs(int channel, const float* coeffs, int numCoeffs);
  void updateMaxDecayTime(int channel, unsigned long maxDecayTimeMs);
  void updateThreshold(int channel, float thresholdPercent);
  void updateDeadTime(int channel, float deadTimeUs);
  void updateMode(int channel, FilterMode mode);

  // Für COUNT_MODE
  void onPulse(int channel);  // Interrupt-Callback
  unsigned long getCPM(int channel);  // CPM für GM-Zähler

private:
  struct FilterState {
    FilterType type;
    float normalFreqHz;
    unsigned long expectedIntervalMs;
    unsigned long maxDecayTimeMs;
    unsigned long warmUpTimeMs;
    float thresholdPercent;
    float deadTimeUs;
    FilterMode mode;
    unsigned long startTime;
    unsigned long lastPushTime;
    float filteredValue;  // Für EMA
    float baseAlpha;      // Für EMA
    std::vector<float> history;  // Für SMA/FIR
    std::vector<float> baseCoeffs;  // Für SMA/FIR
    volatile unsigned long pulseCount;  // Für COUNT_MODE
  };

  std::vector<FilterState> _filters;  // Eine pro Kanal
  String _sensorId;  // Sensor-ID aus letztem pushSensorData

  void initFilter(FilterState& state, const FilterConfig& config);
  void initSMA(FilterState& state, int length);
  void initFIR(FilterState& state, const float* coeffs, int numCoeffs);
  float calculateDecayFactor(const FilterState& state, unsigned long deltaT) const;
  void updateEMA(FilterState& state, float value, float decayFactor);
  void updateSMAorFIR(FilterState& state, float decayFactor);
  void pushToHistory(FilterState& state, float value);
  void initializeHistory(FilterState& state, float value);
  bool isSignificantChange(const FilterState& state, float value) const;
};

#endif
