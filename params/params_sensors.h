#ifndef PARAMS_SENSORS_H
#define PARAMS_SENSORS_H

#include "DynamicAdaptiveFilterV2.h"
#include "filter_chebyshev.h"
#include "filter_bessel.h"

// Filter-Koeffizienten
const float chebyshev_lowpass[] = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f}; // Chebyshev Ordnung 2, 5 Taps
const float bessel_lowpass[] = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f};   // Bessel Ordnung 2, 5 Taps

// Bosch BME280: Temperatur, Feuchte, Druck
const std::vector<FilterConfig> filter_bme280 = {
  {EMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 2.0f, 0.0f, VALUE_MODE}, // Temperatur (°C), ±0.5°C Rauschen
  {EMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 2.0f, 0.0f, VALUE_MODE}, // Feuchte (%), ±3% Rauschen
  {SMA, 5, nullptr, 0, 0.5f, 86400000, 5000, 1.0f, 0.0f, VALUE_MODE}  // Druck (hPa), ±1 hPa Rauschen
};

// Bosch BME680/BME688: Temperatur, Feuchte, Druck, Gaswiderstand
const std::vector<FilterConfig> filter_bme688 = {
  {EMA, 15, nullptr, 0, 0.5f, 86400000, 5000, 2.0f, 0.0f, VALUE_MODE}, // Temperatur (°C)
  {SMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 2.0f, 0.0f, VALUE_MODE}, // Feuchte (%)
  {FIR, 0, chebyshev_lowpass, 5, 0.5f, 86400000, 5000, 1.0f, 0.0f, VALUE_MODE}, // Druck (hPa)
  {FIR, 0, bessel_lowpass, 5, 1.0f, 86400000, 5000, 10.0f, 0.0f, VALUE_MODE} // Gas (kOhm), verrauscht
};

// Sensirion SHT45: Temperatur, Feuchte
const std::vector<FilterConfig> filter_sht45 = {
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 1.0f, 0.0f, VALUE_MODE}, // Temperatur (°C), ±0.1°C
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 1.0f, 0.0f, VALUE_MODE}  // Feuchte (%), ±1%
};

// Sensirion SHT31: Temperatur, Feuchte
const std::vector<FilterConfig> filter_sht31 = {
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 2.0f, 0.0f, VALUE_MODE}, // Temperatur (°C), ±0.2°C
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 2.0f, 0.0f, VALUE_MODE}  // Feuchte (%), ±2%
};

// u-blox NEO-M10: Lat, Lon, Höhe, Geschwindigkeit, Kurs, Satellitenanzahl
const std::vector<FilterConfig> filter_gps_neo_m10 = {
  {FIR, 0, chebyshev_lowpass, 5, 2.0f, 86400000, 10000, 1.0f, 0.0f, VALUE_MODE}, // Lat (Grad), ±2–5 m
  {FIR, 0, chebyshev_lowpass, 5, 2.0f, 86400000, 10000, 1.0f, 0.0f, VALUE_MODE}, // Lon (Grad)
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 10000, 2.0f, 0.0f, VALUE_MODE},          // Höhe (m), ±3–5 m
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 10000, 5.0f, 0.0f, VALUE_MODE},      // Geschwindigkeit (km/h)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 10000, 5.0f, 0.0f, VALUE_MODE},      // Kurs (Grad)
  {SMA, 5, nullptr, 0, 2.0f, 86400000, 10000, 0.0f, 0.0f, VALUE_MODE}             // Satellitenanzahl
};

#endif