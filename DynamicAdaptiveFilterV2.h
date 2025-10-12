#ifndef DYNAMIC_ADAPTIVE_FILTER_V2_H
#define DYNAMIC_ADAPTIVE_FILTER_V2_H

#include <Arduino.h>
#include <vector>
#include <string>

#define MAX_FILTER_LENGTH 5 // Maximale Filterlänge für LMS/RLS

// Makro-Logik: Verhindere Kombinationen von Kalman, LMS und RLS
#if defined(USE_KALMAN) && defined(USE_LMS)
#error "Cannot use KALMAN and LMS together"
#endif
#if defined(USE_KALMAN) && defined(USE_RLS)
#error "Cannot use KALMAN and RLS together"
#endif
#if defined(USE_LMS) && defined(USE_RLS)
#error "Cannot use LMS and RLS together"
#endif

// Filtertypen
enum FilterType {
  EMA,  // Exponential Moving Average
  SMA,  // Simple Moving Average
  FIR,  // Finite Impulse Response
#if defined(USE_KALMAN)
  KALMAN,
#endif
#if defined(USE_LMS)
  LMS,
#endif
#if defined(USE_RLS)
  RLS,
#endif
};

// Filtermodi
enum FilterMode {
  VALUE_MODE,  // Für kontinuierliche Werte (z.B. ADC)
  COUNT_MODE   // Für Zählimpulse (z.B. GM)
};

// Konfigurationsstruktur für jeden Messkanal
struct FilterConfig {
  FilterType type;
  int length;                   // Für EMA/SMA: Fensterlänge; FIR: Ignoriert
  const float* coeffs;          // Für FIR: Koeffizienten
  int numCoeffs;                // Für FIR: Anzahl Koeffizienten
  float normalFreqHz;           // Normale Frequenz (Hz)
  unsigned long maxDecayTimeMs; // Max. Decay-Zeit (ms)
  unsigned long warmUpTimeMs;   // Warm-up-Zeit (ms)
  float thresholdPercent;       // Schmitt-Trigger-Threshold (%)
  float deadTimeUs;             // Dead Time (µs, für COUNT_MODE)
  FilterMode mode;              // VALUE_MODE oder COUNT_MODE
  float madThreshold;           // Schwellwert für MAD (z.B. 3.0)
#if defined(USE_KALMAN)
  float Q;                      // Prozessrauschen
  float R;                      // Messrauschen
  float initialState;           // Initialzustand
#endif
#if defined(USE_LMS)
  float mu;                     // Lernrate
#endif
#if defined(USE_RLS)
  float lambda;                 // Forget-Factor
#endif
};

// Sensor-Datenstruktur
struct SensorData {
  std::vector<float> values;     // Messwerte (z.B. [Temp, Feuchte, Druck])
  std::vector<float> referenceValues; // Für LMS/RLS: Referenzsignal
  unsigned long timestamp;      // Zeitstempel (millis())
  String sensorId;              // Sensor-ID (z.B. "BME688")
};

class DynamicAdaptiveFilterV2 {
public:
  DynamicAdaptiveFilterV2(const std::vector<FilterConfig>& configs);
  void begin(); // Neue Methode für Initialisierung
  bool pushSensorData(const SensorData& data);
  std::vector<float> getFilteredValues() const;
  void updateNormalFreq(int channel, float normalFreqHz);
  void updateLength(int channel, int length);
  void updateFIRCoeffs(int channel, const float* coeffs, int numCoeffs);
  void updateMaxDecayTime(int channel, unsigned long maxDecayTimeMs);
  void updateThreshold(int channel, float thresholdPercent);
  void updateDeadTime(int channel, float deadTimeUs);
  void updateMode(int channel, FilterMode mode);
  void onPulse(int channel);
  unsigned long getCPM(int channel);

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
#if defined(USE_KALMAN)
    float P;              // Kovarianz
    float x;              // Zustand
#endif
#if defined(USE_LMS)
    float coeffs[MAX_FILTER_LENGTH];
    float inputBuffer[MAX_FILTER_LENGTH];
    int bufferIndex;
#endif
#if defined(USE_RLS)
    float coeffs[MAX_FILTER_LENGTH];
    float inputBuffer[MAX_FILTER_LENGTH];
    int bufferIndex;
    float P;              // Kovarianz
#endif
  };

  std::vector<FilterState> _filters;
  std::vector<FilterConfig> _configs;
  String _sensorId;

  bool validateConfig(const FilterConfig& config);
  void initFilter(FilterState& state, const FilterConfig& config);
  void initSMA(FilterState& state, int length);
  void initFIR(FilterState& state, const float* coeffs, int numCoeffs);
  float calculateDecayFactor(const FilterState& state, unsigned long deltaT) const;
  void updateEMA(FilterState& state, float value, float decayFactor);
  void updateSMAorFIR(FilterState& state, float decayFactor);
  void pushToHistory(FilterState& state, float value);
  void initializeHistory(FilterState& state, float value);
  bool isSignificantChange(const FilterState& state, float value) const;
  float calculateMAD(float* data, int windowSize); // Neu: MAD-Berechnung
};

#endif
