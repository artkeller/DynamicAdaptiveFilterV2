#ifndef PARAMS_GMCT_H
#define PARAMS_GMCT_H

// Struktur für GM-Rohr-Parameter
struct GMCT_Params {
  float deadTimeUs;                // Dead Time in Mikrosekunden
  float cpmToMicroSvPerHour;       // Konversionsfaktor CPM zu µSv/h
  float recommendedThresholdPercent; // Empfohlener Schmitt-Trigger-Threshold (%)
  float recommendedNormalFreqHz;    // Empfohlene Normalfrequenz (Hz, z.B. 1/60 für CPM)
  int recommendedLength;            // Empfohlene Filterlänge (für EMA/SMA)
};

// Definitionen für gängige GM-Rohre
const GMCT_Params GMCT_SBM20 = {
  100.0f,         // Dead Time: 100 µs (typisch für SBM-20, basierend auf Datenblättern)
  1.0f / 220.0f,  // 220 CPM = 1 µSv/h (GQ Electronics, SBM-20 Kalibrierung)
  5.0f,           // Threshold: 5% für Rauschunterdrückung (Poisson: σ=√CPM)
  1.0f / 60.0f,   // Normalfrequenz: 1 CPM-Intervall (60 s)
  10              // EMA-Länge: ~10 Minuten Glättung
};

const GMCT_Params GMCT_J305 = {
  80.0f,          // Dead Time: 80 µs (geschätzt, ähnlich SBM-20)
  1.0f / 200.0f,  // 200 CPM = 1 µSv/h (typisch für J305, variiert je nach Kalibrierung)
  5.0f,           // Threshold: 5%
  1.0f / 60.0f,   // Normalfrequenz: 1 CPM
  10              // EMA-Länge: 10
};

const GMCT_Params GMCT_STS5 = {
  120.0f,         // Dead Time: 120 µs (STS-5, älteres sowjetisches Rohr)
  1.0f / 240.0f,  // 240 CPM = 1 µSv/h (geschätzt aus DIY-Projekten)
  5.0f,           // Threshold: 5%
  1.0f / 60.0f,   // Normalfrequenz: 1 CPM
  10              // EMA-Länge: 10
};

const GMCT_Params GMCT_LND712 = {
  60.0f,          // Dead Time: 60 µs (LND-712, sensitiver für Alpha/Gamma)
  1.0f / 175.0f,  // 175 CPM = 1 µSv/h (LND-Datenblatt, kalibriert)
  3.0f,           // Threshold: 3% (höhere Sensitivität, weniger Rauschen)
  1.0f / 60.0f,   // Normalfrequenz: 1 CPM
  8               // EMA-Länge: 8 (schnellere Reaktion für Alpha)
};

#endif