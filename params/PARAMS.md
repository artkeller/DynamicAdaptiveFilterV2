# PARAMS.md – Vordefinierte Sensorkonfigurationen

Diese Datei beschreibt vordefinierte Filterparameter (`FilterConfig`) für eine Auswahl gängiger Sensoren.
Jede Konfiguration ist direkt einsatzbereit und kann ohne tiefes Wissen über Filtertheorie verwendet werden.

Die vorkonfigurierten Parameter orientieren sich an:

* **typischen Messfehlern** (z. B. ±0,5 °C bei Temperatursensoren)
* **normalen Abtastraten** der jeweiligen Sensoren
* **optimierter Signalglättung**, ohne wichtige Details oder Ereignisse zu verfälschen

Ziel ist es, **einfach nutzbare Standardprofile** bereitzustellen, die in den meisten Projekten sofort funktionieren.

> **Hinweis:**
> Die Parameter können jederzeit im Code angepasst oder dynamisch zur Laufzeit geändert werden, falls ein Sensor in einer speziellen Umgebung genutzt wird.

---

## 📂 Übersicht

Die Sensorkonfigurationen sind nach Sensortypen gruppiert:

1. [Temperatur- und Feuchtigkeitssensoren](#1-temperatur--und-feuchtigkeitssensoren)
2. [Drucksensoren](#2-drucksensoren)
3. [Umweltsensoren (Kombisensoren)](#3-umweltsensoren-kombisensoren)
4. [Gassensoren (CO2, VOC, Luftqualität)](#4-gassensoren-co2-voc-luftqualität)
5. [Feinstaubsensoren](#5-feinstaubsensoren)
6. [Licht- und UV-Sensoren](#6-licht--und-uv-sensoren)
7. [IMU-Sensoren (Bewegung, Lage, Orientierung)](#7-imu-sensoren-bewegung-lage-orientierung)
8. [GPS/GNSS-Module](#8-gpsgnss-module)
9. [Zählsensoren (Impulszählung, Geiger-Müller)](#9-zählsensoren-impulszählung-geiger-müller)

---

## 1. Temperatur- und Feuchtigkeitssensoren

Diese Sensoren liefern relativ **langsame Signale**, daher werden **EMA-Filter** verwendet.
Die Filter glätten Messrauschen und reduzieren kleine Schwankungen, ohne Verzögerungen einzuführen.

| Sensor                                   | Typischer Einsatz                | Filter                                    |
| ---------------------------------------- | -------------------------------- | ----------------------------------------- |
| SHT10, SHT11, SHT15                      | Legacy-Modelle                   | EMA, sanfte Glättung (±0,3–0,5 °C)        |
| SHT20, SHT21, SHT25                      | Standardmodelle                  | EMA, sanfte Glättung (±0,2–0,3 °C)        |
| SHT30, SHT31, SHT35, SHT40, SHT41, SHT45 | Moderne Sensirion-Modelle        | EMA, optimiert für sehr präzise Messungen |
| DS18B20                                  | 1-Wire Temperaturfühler          | EMA, starke Glättung für ±0,5 °C          |
| TMP117                                   | Hochpräziser TI-Temperatursensor | EMA, sehr geringe Schwankungen (±0,1 °C)  |
| LM75                                     | Legacy I²C Sensor                | EMA, Grundglättung (±2 °C)                |

> **Warum EMA:**
> Temperatur ändert sich langsam, daher reicht ein exponentieller gleitender Mittelwert für eine sanfte Glättung völlig aus.

---

## 2. Drucksensoren

Drucksensoren benötigen eine **stärkere Glättung**, da Luftdruckmessungen anfällig für Rauschen sind.

| Sensor | Filter          | Bemerkung            |
| ------ | --------------- | -------------------- |
| BMP180 | SMA (Fenster 5) | Legacy-Modell        |
| BMP280 | SMA (Fenster 5) | ±1 hPa Genauigkeit   |
| BMP390 | SMA (Fenster 5) | ±0,5 hPa Genauigkeit |

> **Warum SMA:**
> Ein einfacher gleitender Mittelwert ist ideal für barometrische Trends und Höhenbestimmungen.

---

## 3. Umweltsensoren (Kombisensoren)

Sensoren wie die Bosch BME-Serie messen mehrere Größen gleichzeitig: Temperatur, Feuchtigkeit, Druck und teilweise Gas.

| Sensor     | Kanäle                          | Filter                         |
| ---------- | ------------------------------- | ------------------------------ |
| **BME280** | Temperatur, Feuchte, Druck      | EMA (T, H), SMA (P)            |
| **BME680** | Temperatur, Feuchte, Druck, Gas | EMA (T), SMA (H), FIR (P, Gas) |
| **BME688** | Temperatur, Feuchte, Druck, Gas | EMA (T), SMA (H), FIR (P, Gas) |

> **FIR für Gas/Druck:**
> Gas- und Druckmessungen können stark schwanken, daher werden stabile FIR-Filter genutzt, um Ausreißer zu unterdrücken.

---

## 4. Gassensoren (CO2, VOC, Luftqualität)

Diese Sensoren reagieren empfindlich auf Umwelteinflüsse und erfordern **FIR-Filter**, um langsame Trends sichtbar zu machen.

| Sensor | Kanäle                   | Filter                |
| ------ | ------------------------ | --------------------- |
| SGP30  | eCO2, TVOC               | FIR (Bessel Lowpass)  |
| SCD30  | CO2, Temperatur, Feuchte | SMA (CO2), EMA (T, H) |
| CCS811 | eCO2, TVOC               | FIR (Bessel Lowpass)  |

> **Besonderheit:**
> Gassensoren können starke kurzzeitige Peaks zeigen, die nicht relevant sind. FIR-Filter sorgen für eine gleichmäßige Trendanzeige.

---

## 5. Feinstaubsensoren

Feinstaubwerte schwanken stark, daher werden **SMA-Filter** mit Fenstergrößen von 10 verwendet.

| Sensor            | Kanäle             | Filter           |
| ----------------- | ------------------ | ---------------- |
| Plantower PMS5003 | PM1.0, PM2.5, PM10 | SMA (Fenster 10) |
| Sensirion SPS30   | PM1.0, PM2.5, PM10 | SMA (Fenster 10) |

---

## 6. Licht- und UV-Sensoren

| Sensor   | Typische Einheit | Filter |
| -------- | ---------------- | ------ |
| BH1750   | Lux              | EMA    |
| TSL2591  | Lux              | EMA    |
| VEML6075 | UVA, UVB         | EMA    |

> **Warum EMA:**
> Lichtverhältnisse ändern sich oft schlagartig. EMA reagiert schnell auf Änderungen, ohne stark zu rauschen.

---

## 7. IMU-Sensoren (Bewegung, Lage, Orientierung)

IMUs messen Beschleunigung, Gyroskop und Magnetometer.
Für diese hochfrequenten Signale werden **FIR-Filter** mit **Bessel-Koeffizienten** verwendet, um Verzerrungen zu vermeiden.

| Sensor             | Kanäle                      | Bemerkung               |
| ------------------ | --------------------------- | ----------------------- |
| MPU-6050           | 6 Kanäle (Acc + Gyro)       | Legacy-Modell           |
| MPU-9150, MPU-9250 | 9 Kanäle (Acc + Gyro + Mag) | Magnetometer zusätzlich |
| ICM-20948          | 9 Kanäle                    | Moderner Nachfolger     |
| BNO055             | 9 Kanäle                    | Integrierte Fusion      |

> **Warum FIR/Bessel:**
> Bewegungsdaten müssen ohne Phasenverschiebung verarbeitet werden – wichtig für Robotik, Drohnen und VR.

---

## 8. GPS/GNSS-Module

GPS-Daten schwanken stark, vor allem Geschwindigkeit und Kurs.
Hier kommen **FIR-Filter** für Positionsdaten und **EMA/SMA** für langsamere Signale wie Satellitenanzahl zum Einsatz.

| Sensor                  | Filter                                                             |
| ----------------------- | ------------------------------------------------------------------ |
| NEO-M8, NEO-M9, NEO-M10 | FIR (Lat/Lon, Geschwindigkeit, Kurs), EMA (Höhe), SMA (Satelliten) |

> **Beispiel:**
>
> * FIR-Filter entfernt GPS-Rauschen aus der Position
> * EMA glättet Höhenangaben
> * SMA sorgt für stabile Anzeige der Satellitenanzahl

---

## 9. Zählsensoren (Impulszählung, Geiger-Müller)

Zählsensoren wie **Geiger-Müller-Zählrohre** arbeiten im **COUNT\_MODE**.

| Sensor               | Einheit   | Filter                   |
| -------------------- | --------- | ------------------------ |
| GMCT (Geiger-Müller) | CPM / CPS | EMA mit Totzeitkorrektur |

> **Besonderheit:**
>
> * Jeder Impuls wird per Interrupt erfasst (`onPulse()`)
> * Der Filter glättet die Counts per Minute (CPM) und korrigiert Totzeiten automatisch

---

## 🧾 Zusammenfassung

Die vorkonfigurierten Parameter bieten:

* **Direkte Einsatzbereitschaft** für viele gängige Sensoren
* **Optimierte Filtereinstellungen**, angepasst an Messverhalten und Rauschprofile
* Eine solide Grundlage für individuelle Anpassungen in Spezialprojekten

Für komplexe Anwendungsfälle können die Parameter im Code einfach überschrieben oder dynamisch geändert werden, z. B. um auf wechselnde Umgebungsbedingungen zu reagieren.
