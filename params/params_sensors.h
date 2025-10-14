#ifndef PARAMS_SENSORS_H
#define PARAMS_SENSORS_H

#include "DynamicAdaptiveFilterV2.h"
#include "filter/FIR_coefficients.h"

// Filter-Koeffizienten
const float chebyshev_lowpass[] = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f}; // Chebyshev Ordnung 2, 5 Taps
const float bessel_lowpass[] = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f};    // Bessel Ordnung 2, 5 Taps

// Temp/Humidity: Sensirion SHT10 (Legacy)
const std::vector<FilterConfig> filter_sht10 = {
  {EMA, 10, nullptr, 0, 0.5f, 86400000, 2000, 3.0f, 0.0f, VALUE_MODE}, // Temperatur (°C), ±0.5°C
  {EMA, 10, nullptr, 0, 0.5f, 86400000, 2000, 3.0f, 0.0f, VALUE_MODE}  // Feuchte (%), ±4%
};

// Temp/Humidity: Sensirion SHT11 (Legacy)
const std::vector<FilterConfig> filter_sht11 = {
  {EMA, 10, nullptr, 0, 0.5f, 86400000, 2000, 3.0f, 0.0f, VALUE_MODE}, // Temperatur (°C), ±0.4°C
  {EMA, 10, nullptr, 0, 0.5f, 86400000, 2000, 3.0f, 0.0f, VALUE_MODE}  // Feuchte (%), ±3%
};

// Temp/Humidity: Sensirion SHT15 (Legacy)
const std::vector<FilterConfig> filter_sht15 = {
  {EMA, 10, nullptr, 0, 0.5f, 86400000, 2000, 3.0f, 0.0f, VALUE_MODE}, // Temperatur (°C), ±0.3°C
  {EMA, 10, nullptr, 0, 0.5f, 86400000, 2000, 3.0f, 0.0f, VALUE_MODE}  // Feuchte (%), ±2%
};

// Temp/Humidity: Sensirion SHT20 (Legacy)
const std::vector<FilterConfig> filter_sht20 = {
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 2.0f, 0.0f, VALUE_MODE}, // Temperatur (°C), ±0.3°C
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 2.0f, 0.0f, VALUE_MODE}  // Feuchte (%), ±3%
};

// Temp/Humidity: Sensirion SHT21 (Legacy)
const std::vector<FilterConfig> filter_sht21 = {
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 2.0f, 0.0f, VALUE_MODE}, // Temperatur (°C), ±0.3°C
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 2.0f, 0.0f, VALUE_MODE}  // Feuchte (%), ±2%
};

// Temp/Humidity: Sensirion SHT25 (Legacy)
const std::vector<FilterConfig> filter_sht25 = {
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 2.0f, 0.0f, VALUE_MODE}, // Temperatur (°C), ±0.2°C
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 2.0f, 0.0f, VALUE_MODE}  // Feuchte (%), ±1.8%
};

// Temp/Humidity: Sensirion SHT30
const std::vector<FilterConfig> filter_sht30 = {
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 2.0f, 0.0f, VALUE_MODE}, // Temperatur (°C), ±0.2°C
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 2.0f, 0.0f, VALUE_MODE}  // Feuchte (%), ±2%
};

// Temp/Humidity: Sensirion SHT31
const std::vector<FilterConfig> filter_sht31 = {
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 2.0f, 0.0f, VALUE_MODE}, // Temperatur (°C), ±0.2°C
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 2.0f, 0.0f, VALUE_MODE}  // Feuchte (%), ±2%
};

// Temp/Humidity: Sensirion SHT35
const std::vector<FilterConfig> filter_sht35 = {
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 1.5f, 0.0f, VALUE_MODE}, // Temperatur (°C), ±0.1°C
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 1.5f, 0.0f, VALUE_MODE}  // Feuchte (%), ±1.5%
};

// Temp/Humidity: Sensirion SHT40
const std::vector<FilterConfig> filter_sht40 = {
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 1.0f, 0.0f, VALUE_MODE}, // Temperatur (°C), ±0.2°C
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 1.0f, 0.0f, VALUE_MODE}  // Feuchte (%), ±1.8%
};

// Temp/Humidity: Sensirion SHT41
const std::vector<FilterConfig> filter_sht41 = {
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 1.0f, 0.0f, VALUE_MODE}, // Temperatur (°C), ±0.2°C
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 1.0f, 0.0f, VALUE_MODE}  // Feuchte (%), ±1.8%
};

// Temp/Humidity: Sensirion SHT45
const std::vector<FilterConfig> filter_sht45 = {
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 1.0f, 0.0f, VALUE_MODE}, // Temperatur (°C), ±0.1°C
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 1.0f, 0.0f, VALUE_MODE}  // Feuchte (%), ±1%
};

// Temp/Humidity: Gemeinsam für SHT2x, Si7021, HTU21D
const std::vector<FilterConfig> filter_temp_hum = {
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 2.0f, 0.0f, VALUE_MODE}, // Temperatur (°C), ±0.3–0.4°C
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 2.0f, 0.0f, VALUE_MODE}  // Feuchte (%), ±2–3%
};

// Temperature: Maxim DS18B20
const std::vector<FilterConfig> filter_ds18b20 = {
  {EMA, 10, nullptr, 0, 0.5f, 86400000, 2000, 2.0f, 0.0f, VALUE_MODE}  // Temperatur (°C), ±0.5°C
};

// Temperature: Texas Instruments TMP117
const std::vector<FilterConfig> filter_tmp117 = {
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 1.0f, 0.0f, VALUE_MODE}  // Temperatur (°C), ±0.1°C
};

// Temperature: Texas Instruments LM75 (Legacy)
const std::vector<FilterConfig> filter_lm75 = {
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 3.0f, 0.0f, VALUE_MODE}  // Temperatur (°C), ±2°C
};

// Pressure: Bosch BMP180 (Legacy)
const std::vector<FilterConfig> filter_bmp180 = {
  {SMA, 5, nullptr, 0, 0.5f, 86400000, 5000, 2.0f, 0.0f, VALUE_MODE}  // Druck (hPa), ±2 hPa
};

// Pressure: Bosch BMP280
const std::vector<FilterConfig> filter_bmp280 = {
  {SMA, 5, nullptr, 0, 0.5f, 86400000, 5000, 1.0f, 0.0f, VALUE_MODE}  // Druck (hPa), ±1 hPa
};

// Pressure: Bosch BMP390
const std::vector<FilterConfig> filter_bmp390 = {
  {SMA, 5, nullptr, 0, 0.5f, 86400000, 5000, 0.5f, 0.0f, VALUE_MODE}  // Druck (hPa), ±0.5 hPa
};

// Enviro Combo: Bosch BME280
const std::vector<FilterConfig> filter_bme280 = {
  {EMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 2.0f, 0.0f, VALUE_MODE}, // Temperatur (°C), ±0.5°C
  {EMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 2.0f, 0.0f, VALUE_MODE}, // Feuchte (%), ±3%
  {SMA, 5, nullptr, 0, 0.5f, 86400000, 5000, 1.0f, 0.0f, VALUE_MODE}   // Druck (hPa), ±1 hPa
};

// Enviro Combo: Bosch BME680
const std::vector<FilterConfig> filter_bme680 = {
  {EMA, 15, nullptr, 0, 0.5f, 86400000, 5000, 2.0f, 0.0f, VALUE_MODE},          // Temperatur (°C), ±0.5°C
  {SMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 2.0f, 0.0f, VALUE_MODE},          // Feuchte (%), ±3%
  {FIR, 0, chebyshev_lowpass, 5, 0.5f, 86400000, 5000, 1.0f, 0.0f, VALUE_MODE}, // Druck (hPa), ±1 hPa
  {FIR, 0, bessel_lowpass, 5, 1.0f, 86400000, 5000, 10.0f, 0.0f, VALUE_MODE}    // Gas (kOhm), ±10 kOhm
};

// Enviro Combo: Bosch BME688
const std::vector<FilterConfig> filter_bme688 = {
  {EMA, 15, nullptr, 0, 0.5f, 86400000, 5000, 2.0f, 0.0f, VALUE_MODE},           // Temperatur (°C), ±0.5°C
  {SMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 2.0f, 0.0f, VALUE_MODE},           // Feuchte (%), ±3%
  {FIR, 0, chebyshev_lowpass, 5, 0.5f, 86400000, 5000, 1.0f, 0.0f, VALUE_MODE}, // Druck (hPa), ±1 hPa
  {FIR, 0, bessel_lowpass, 5, 1.0f, 86400000, 5000, 10.0f, 0.0f, VALUE_MODE}    // Gas (kOhm), ±10 kOhm
};

// Gemeinsam: BME280/680/688 (Temp, Feuchte, Druck)
const std::vector<FilterConfig> filter_bme = {
  {EMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 2.0f, 0.0f, VALUE_MODE},          // Temperatur (°C), ±0.5°C
  {SMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 2.0f, 0.0f, VALUE_MODE},          // Feuchte (%), ±3%
  {FIR, 0, chebyshev_lowpass, 5, 0.5f, 86400000, 5000, 1.0f, 0.0f, VALUE_MODE}  // Druck (hPa), ±1 hPa
};

// Gas: Sensirion SGP30
const std::vector<FilterConfig> filter_sgp30 = {
  {FIR, 0, bessel_lowpass, 5, 1.0f, 86400000, 5000, 10.0f, 0.0f, VALUE_MODE}, // eCO2 (ppm), ±50 ppm
  {FIR, 0, bessel_lowpass, 5, 1.0f, 86400000, 5000, 10.0f, 0.0f, VALUE_MODE}  // TVOC (ppb), ±10 ppb
};

// Gas: Sensirion SCD30
const std::vector<FilterConfig> filter_scd30 = {
  {SMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 5.0f, 0.0f, VALUE_MODE}, // CO2 (ppm), ±50 ppm
  {EMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 2.0f, 0.0f, VALUE_MODE}, // Temperatur (°C), ±0.5°C
  {EMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 2.0f, 0.0f, VALUE_MODE}  // Feuchte (%), ±3%
};

// Gas: AMS CCS811
const std::vector<FilterConfig> filter_ccs811 = {
  {FIR, 0, bessel_lowpass, 5, 1.0f, 86400000, 5000, 10.0f, 0.0f, VALUE_MODE}, // eCO2 (ppm), ±50 ppm
  {FIR, 0, bessel_lowpass, 5, 1.0f, 86400000, 5000, 10.0f, 0.0f, VALUE_MODE}  // TVOC (ppb), ±10 ppb
};

// Gemeinsam: SGP30, CCS811 (VOC-Sensoren)
const std::vector<FilterConfig> filter_voc = {
  {FIR, 0, bessel_lowpass, 5, 1.0f, 86400000, 5000, 10.0f, 0.0f, VALUE_MODE}, // eCO2 (ppm), ±50 ppm
  {FIR, 0, bessel_lowpass, 5, 1.0f, 86400000, 5000, 10.0f, 0.0f, VALUE_MODE}  // TVOC (ppb), ±10 ppb
};

// Particulate Matter: Plantower PMS5003
const std::vector<FilterConfig> filter_pms5003 = {
  {SMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 5.0f, 0.0f, VALUE_MODE}, // PM1.0 (µg/m³), ±5 µg/m³
  {SMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 5.0f, 0.0f, VALUE_MODE}, // PM2.5 (µg/m³), ±5 µg/m³
  {SMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 5.0f, 0.0f, VALUE_MODE}  // PM10 (µg/m³), ±10 µg/m³
};

// Particulate Matter: Sensirion SPS30
const std::vector<FilterConfig> filter_sps30 = {
  {SMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 3.0f, 0.0f, VALUE_MODE}, // PM1.0 (µg/m³), ±3 µg/m³
  {SMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 3.0f, 0.0f, VALUE_MODE}, // PM2.5 (µg/m³), ±3 µg/m³
  {SMA, 10, nullptr, 0, 0.5f, 86400000, 5000, 3.0f, 0.0f, VALUE_MODE}  // PM10 (µg/m³), ±5 µg/m³
};

// Light: ROHM BH1750
const std::vector<FilterConfig> filter_bh1750 = {
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 5.0f, 0.0f, VALUE_MODE}  // Lux, ±5%
};

// Light: AMS TSL2591
const std::vector<FilterConfig> filter_tsl2591 = {
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 3.0f, 0.0f, VALUE_MODE}  // Lux, ±3%
};

// UV: Vishay VEML6075
const std::vector<FilterConfig> filter_veml6075 = {
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 5.0f, 0.0f, VALUE_MODE}, // UVA
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 5.0f, 0.0f, VALUE_MODE}  // UVB
};

// IMU: InvenSense MPU-6050 (Legacy)
const std::vector<FilterConfig> filter_mpu6050 = {
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc X (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc Y (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc Z (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro X (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro Y (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}  // Gyro Z (°/s)
};

// IMU: InvenSense MPU-9150 (Legacy)
const std::vector<FilterConfig> filter_mpu9150 = {
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc X (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc Y (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc Z (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro X (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro Y (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro Z (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Mag X (µT)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Mag Y (µT)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}  // Mag Z (µT)
};

// IMU: InvenSense MPU-9250 (Legacy)
const std::vector<FilterConfig> filter_mpu9250 = {
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc X (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc Y (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc Z (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro X (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro Y (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro Z (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Mag X (µT)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Mag Y (µT)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}  // Mag Z (µT)
};

// IMU: InvenSense ICM-20948
const std::vector<FilterConfig> filter_icm20948 = {
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc X (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc Y (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc Z (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro X (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro Y (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro Z (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Mag X (µT)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Mag Y (µT)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}  // Mag Z (µT)
};

// IMU: Bosch BNO055
const std::vector<FilterConfig> filter_bno055 = {
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc X (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc Y (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc Z (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro X (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro Y (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro Z (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Mag X (µT)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Mag Y (µT)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}  // Mag Z (µT)
};

// Gemeinsam: IMU 6-Axis (z. B. MPU-6050)
const std::vector<FilterConfig> filter_imu_6axis = {
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc X (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc Y (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc Z (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro X (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro Y (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}  // Gyro Z (°/s)
};

// Gemeinsam: IMU 9-Axis (z. B. MPU-9150, MPU-9250, ICM-20948, BNO055)
const std::vector<FilterConfig> filter_imu_9axis = {
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc X (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc Y (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Acc Z (g)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro X (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro Y (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Gyro Z (°/s)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Mag X (µT)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}, // Mag Y (µT)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE}  // Mag Z (µT)
};

// GNSS: u-blox NEO-M8 (Legacy)
const std::vector<FilterConfig> filter_gps_neo_m8 = {
  {FIR, 0, chebyshev_lowpass, 5, 2.0f, 86400000, 10000, 1.0f, 0.0f, VALUE_MODE}, // Lat (Grad), ±2.5 m
  {FIR, 0, chebyshev_lowpass, 5, 2.0f, 86400000, 10000, 1.0f, 0.0f, VALUE_MODE}, // Lon (Grad)
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 10000, 2.0f, 0.0f, VALUE_MODE},          // Höhe (m), ±5 m
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 10000, 5.0f, 0.0f, VALUE_MODE},    // Geschwindigkeit (km/h)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 10000, 5.0f, 0.0f, VALUE_MODE},    // Kurs (Grad)
  {SMA, 5, nullptr, 0, 2.0f, 86400000, 10000, 0.0f, 0.0f, VALUE_MODE}            // Satellitenanzahl
};

// GNSS: u-blox NEO-M9
const std::vector<FilterConfig> filter_gps_neo_m9 = {
  {FIR, 0, chebyshev_lowpass, 5, 2.0f, 86400000, 10000, 1.0f, 0.0f, VALUE_MODE}, // Lat (Grad), ±2 m
  {FIR, 0, chebyshev_lowpass, 5, 2.0f, 86400000, 10000, 1.0f, 0.0f, VALUE_MODE}, // Lon (Grad)
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 10000, 2.0f, 0.0f, VALUE_MODE},          // Höhe (m), ±3 m
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 10000, 5.0f, 0.0f, VALUE_MODE},    // Geschwindigkeit (km/h)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 10000, 5.0f, 0.0f, VALUE_MODE},    // Kurs (Grad)
  {SMA, 5, nullptr, 0, 2.0f, 86400000, 10000, 0.0f, 0.0f, VALUE_MODE}            // Satellitenanzahl
};

// GNSS: u-blox NEO-M10
const std::vector<FilterConfig> filter_gps_neo_m10 = {
  {FIR, 0, chebyshev_lowpass, 5, 2.0f, 86400000, 10000, 1.0f, 0.0f, VALUE_MODE}, // Lat (Grad), ±2.5 m
  {FIR, 0, chebyshev_lowpass, 5, 2.0f, 86400000, 10000, 1.0f, 0.0f, VALUE_MODE}, // Lon (Grad)
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 10000, 2.0f, 0.0f, VALUE_MODE},          // Höhe (m), ±3–5 m
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 10000, 5.0f, 0.0f, VALUE_MODE},    // Geschwindigkeit (km/h)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 10000, 5.0f, 0.0f, VALUE_MODE},    // Kurs (Grad)
  {SMA, 5, nullptr, 0, 2.0f, 86400000, 10000, 0.0f, 0.0f, VALUE_MODE}            // Satellitenanzahl
};

// Gemeinsam: GNSS NEO-M8, NEO-M9, NEO-M10
const std::vector<FilterConfig> filter_gps_neo = {
  {FIR, 0, chebyshev_lowpass, 5, 2.0f, 86400000, 10000, 1.0f, 0.0f, VALUE_MODE}, // Lat (Grad), ±2–2.5 m
  {FIR, 0, chebyshev_lowpass, 5, 2.0f, 86400000, 10000, 1.0f, 0.0f, VALUE_MODE}, // Lon (Grad)
  {EMA, 10, nullptr, 0, 1.0f, 86400000, 10000, 2.0f, 0.0f, VALUE_MODE},          // Höhe (m), ±3–5 m
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 10000, 5.0f, 0.0f, VALUE_MODE},    // Geschwindigkeit (km/h)
  {FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 10000, 5.0f, 0.0f, VALUE_MODE},    // Kurs (Grad)
  {SMA, 5, nullptr, 0, 2.0f, 86400000, 10000, 0.0f, 0.0f, VALUE_MODE}            // Satellitenanzahl
};


#endif

