#ifndef PARAMS_GMCT_H
#define PARAMS_GMCT_H

#include "DynamicAdaptiveFilterV2.h"

// Struktur für GM-Rohr-Parameter
struct GMCT_Params {
  float deadTimeUs;                // Dead Time in Mikrosekunden
  float cpmToMicroSvPerHour;       // Konversionsfaktor CPM zu µSv/h
  float recommendedThresholdPercent; // Empfohlener Schwellwert (%)
  float recommendedNormalFreqHz;    // Empfohlene Normalfrequenz (Hz)
  int recommendedLength;            // Empfohlene Filterlänge
};

// Definitionen für gängige GM-Rohre
const GMCT_Params GMCT_SBM20 = {
  100.0f,         // Dead Time: 100 µs
  1.0f / 220.0f,  // 220 CPM = 1 µSv/h
  5.0f,           // Threshold: 5%
  1.0f / 60.0f,   // Normalfrequenz: 1 CPM (60 s)
  10              // EMA-Länge: ~10 Minuten
};

const GMCT_Params GMCT_J305 = {
  80.0f,          // Dead Time: 80 µs
  1.0f / 200.0f,  // 200 CPM = 1 µSv/h
  5.0f,           // Threshold: 5%
  1.0f / 60.0f,   // Normalfrequenz: 1 CPM
  10              // EMA-Länge: 10
};

const GMCT_Params GMCT_STS5 = {
  120.0f,         // Dead Time: 120 µs
  1.0f / 240.0f,  // 240 CPM = 1 µSv/h
  5.0f,           // Threshold: 5%
  1.0f / 60.0f,   // Normalfrequenz: 1 CPM
  10              // EMA-Länge: 10
};

const GMCT_Params GMCT_LND712 = {
  60.0f,          // Dead Time: 60 µs
  1.0f / 175.0f,  // 175 CPM = 1 µSv/h
  3.0f,           // Threshold: 3%
  1.0f / 60.0f,   // Normalfrequenz: 1 CPM
  8               // EMA-Länge: 8
};

// Filterkonfigurationen für GM-Rohre
const std::vector<FilterConfig> filter_sbm20 = {
  {EMA, GMCT_SBM20.recommendedLength, nullptr, 0, GMCT_SBM20.recommendedNormalFreqHz, 10000, 1000, GMCT_SBM20.recommendedThresholdPercent, 0.0f, VALUE_MODE}, // CPM
  {EMA, GMCT_SBM20.recommendedLength, nullptr, 0, GMCT_SBM20.recommendedNormalFreqHz, 10000, 1000, GMCT_SBM20.recommendedThresholdPercent, 0.0f, VALUE_MODE}  // µSv/h
};

const std::vector<FilterConfig> filter_j305 = {
  {EMA, GMCT_J305.recommendedLength, nullptr, 0, GMCT_J305.recommendedNormalFreqHz, 10000, 1000, GMCT_J305.recommendedThresholdPercent, 0.0f, VALUE_MODE}, // CPM
  {EMA, GMCT_J305.recommendedLength, nullptr, 0, GMCT_J305.recommendedNormalFreqHz, 10000, 1000, GMCT_J305.recommendedThresholdPercent, 0.0f, VALUE_MODE}  // µSv/h
};

const std::vector<FilterConfig> filter_sts5 = {
  {EMA, GMCT_STS5.recommendedLength, nullptr, 0, GMCT_STS5.recommendedNormalFreqHz, 10000, 1000, GMCT_STS5.recommendedThresholdPercent, 0.0f, VALUE_MODE}, // CPM
  {EMA, GMCT_STS5.recommendedLength, nullptr, 0, GMCT_STS5.recommendedNormalFreqHz, 10000, 1000, GMCT_STS5.recommendedThresholdPercent, 0.0f, VALUE_MODE}  // µSv/h
};

const std::vector<FilterConfig> filter_lnd712 = {
  {EMA, GMCT_LND712.recommendedLength, nullptr, 0, GMCT_LND712.recommendedNormalFreqHz, 10000, 1000, GMCT_LND712.recommendedThresholdPercent, 0.0f, VALUE_MODE}, // CPM
  {EMA, GMCT_LND712.recommendedLength, nullptr, 0, GMCT_LND712.recommendedNormalFreqHz, 10000, 1000, GMCT_LND712.recommendedThresholdPercent, 0.0f, VALUE_MODE}  // µSv/h
};

#endif