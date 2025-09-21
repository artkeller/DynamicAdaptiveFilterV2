# DynamicAdaptiveFilterV2 *** IN PROGRESS --- EXAMPLES BELONG TO EARLIER VERSION ****
DynamicAdaptiveFilterV2 is a versatile Arduino/C++ library for **real-time filtering and smoothing of sensor data and analog signals**.
It supports adaptive filters such as EMA, SMA, and FIR and is ideal for projects that require processing of **noisy, irregular, or impulse-like signals**, e.g., environmental sensors (BME680, SHT31), potentiometers, or voltage measurements (Vbus/Vcc).

---

## 🌍 Typische Anwendungsfälle
- **Umweltsensoren**: Temperatur, Feuchtigkeit, Luftqualität  
- **Bewegungssensoren (IMU)**: Beschleunigung, Gyroskop, Magnetometer  
- **GPS/GNSS-Module**: Positions- und Geschwindigkeitsdaten  
- **Audio**: Mikrofone, Ultraschallsensoren  
- **Strahlung**: Geiger-Müller-Zählrohre  
- **Analoge Eingänge (ADC)**: Allgemeine Messsignale

Die Bibliothek unterstützt:
- **Klassische Filter**: EMA, SMA, FIR  
- **Adaptive Filter**: Kalman, LMS, RLS  

Damit kannst du Signale automatisch optimieren, **ohne dich tief in DSP (Digital Signal Processing) einarbeiten zu müssen**.

---

## 🚀 Hauptfunktionen
- **Echtzeit-Filterung** mehrerer Kanäle gleichzeitig
- **Dynamisch adaptiv** – Parameter passen sich automatisch an wechselnde Bedingungen an
- Unterstützt **kontinuierliche Messwerte** (z. B. Temperatur) und **Impulszähler** (z. B. Geiger-Müller)
- **Konfigurierbar über `FilterConfig`**
- Laufzeit-Updates der Filterparameter:
  - Grenzfrequenz ändern: `updateNormalFreq()`
  - Fenstergröße anpassen: `updateLength()`
  - Filterkoeffizienten austauschen: `updateFIRCoeffs()`
  - Thresholds und Totzeiten ändern: `updateThreshold()`, `updateDeadTime()`
- **Interrupts für COUNT_MODE** (z. B. Geiger-Müller-Pulse)
- Kompatibel mit **Arduino**, **ESP32** (**RP2040** not tested, **AVR-Boards** not adapted yet) usw.

---

## 📦 Installation

1. Lade die Dateien herunter:
   - `DynamicAdaptiveFilterV2.cpp`
   - `DynamicAdaptiveFilterV2.h`

2. Lege sie in deinen Arduino-Projektordner.

3. Binde die Bibliothek im Sketch ein:
```cpp
   #include "DynamicAdaptiveFilterV2.h"
```

4. Stelle sicher, dass deine Entwicklungsumgebung `vector` und `string` unterstützt. (**AVR-Boards** not adapted yet)

---

## ⚙️ Grundkonzept

Jeder Sensorkanal wird durch eine **`FilterConfig`**-Struktur beschrieben.
Ein `DynamicAdaptiveFilterV2`-Objekt verwaltet mehrere Kanäle gleichzeitig.

**Beispiel:**

* Kanal 0 → Temperatur mit EMA-Filter
* Kanal 1 → Beschleunigung mit FIR-Filter
* Kanal 2 → Geiger-Müller-Zähler (COUNT\_MODE)

> Ergebnis: Alle Werte sind sofort **gefiltert** und können mit `getFilteredValues()` abgerufen werden.

---

## 🔢 Unterstützte Filtertypen

| Filtertyp                            | Beschreibung                                | Typische Nutzung                 |
| ------------------------------------ | ------------------------------------------- | -------------------------------- |
| **EMA** (Exponential Moving Average) | Gewichteter gleitender Mittelwert           | Temperatur, Luftfeuchtigkeit     |
| **SMA** (Simple Moving Average)      | Einfacher Durchschnitt über ein Zeitfenster | Drucksensoren, Feinstaub         |
| **FIR** (Finite Impulse Response)    | Flexible Frequenzfilterung                  | IMU, Audio, GPS                  |
| **Kalman**                           | Modellbasiert, optimal für Rauschen         | GPS-Tracking, Sensorfusion       |
| **LMS** (Least Mean Squares)         | Selbstlernender adaptiver Filter            | Rauschunterdrückung, Brummfilter |
| **RLS** (Recursive Least Squares)    | Schneller adaptiver Filter, sehr präzise    | Hochpräzise Messsysteme          |

> ⚠️ **Hinweis:**
> Aus Performancegründen kann **nur ein adaptiver Filtertyp** gleichzeitig aktiviert werden (`Kalman`, `LMS` oder `RLS`).

---

## 📊 Filtermodi

| Modus           | Beschreibung              | Beispiel                     |
| --------------- | ------------------------- | ---------------------------- |
| **VALUE\_MODE** | Für kontinuierliche Werte | Temperatur, ADC              |
| **COUNT\_MODE** | Für Impulszähler          | Geiger-Müller, Puls-Sensoren |

---

## 🧩 Beispiel: Temperatur + Geiger-Müller

Hier wird **ein Temperaturkanal** mit EMA gefiltert und **ein Geiger-Müller-Kanal** im COUNT\_MODE betrieben.

```cpp
#include "DynamicAdaptiveFilterV2.h"

// Temperaturfilter
FilterConfig tempConfig = {
  EMA, 5, nullptr, 0, 1.0f, 5000, 1000, 5.0f, 0.0f, VALUE_MODE
};

// Geiger-Müller-Filter
FilterConfig gmConfig = {
  EMA, 5, nullptr, 0, 1.0f, 60000, 60000, 0.0f, 1000.0f, COUNT_MODE
};

DynamicAdaptiveFilterV2 filter({tempConfig, gmConfig});

void loop() {
  // Temperatur lesen
  SensorData data = {{analogRead(A0)}, millis(), "TEMP"};
  filter.pushSensorData(data);

  // Gefilterte Werte ausgeben
  auto values = filter.getFilteredValues();
  Serial.println(values[0]); // Gefilterte Temperatur
}
```
```cpp
// Beispiel für Temperatur (EMA) und Geiger-Zähler (COUNT_MODE)
// Optional: Kalman oder LMS aktivieren (nur eines!)
//#define USE_KALMAN
//#define USE_LMS
#include "DynamicAdaptiveFilterV2.h"
#include "params_GMCT.h" // Vordefinierte Parameter für Geiger-Zähler

// Temperaturfilter (EMA, VALUE_MODE)
FilterConfig tempConfig = {
  EMA, 5, nullptr, 0, 1.0f, 5000, 1000, 5.0f, 0.0f, VALUE_MODE
};

// Geiger-Zähler (EMA, COUNT_MODE, aus params_GMCT.h)
FilterConfig gmConfig = GM_SBM20_CONFIG; // Beispiel: SBM-20

// Optional: Kalman für GPS-Geschwindigkeit
#if defined(USE_KALMAN)
FilterConfig gpsConfig = {
  KALMAN, 0, nullptr, 0, 10.0f, 10000, 1000, 5.0f, 0.0f, VALUE_MODE, 0.01f, 0.1f, 0.0f
};
#endif

// Optional: LMS für Brummfilter
#if defined(USE_LMS)
FilterConfig brummConfig = {
  LMS, 10, nullptr, 0, 100.0f, 10000, 1000, 5.0f, 0.0f, VALUE_MODE, 0.01f
};
#endif

// Filter initialisieren (je nach Makro)
#if defined(USE_KALMAN)
DynamicAdaptiveFilterV2 filter({tempConfig, gmConfig, gpsConfig});
#elif defined(USE_LMS)
DynamicAdaptiveFilterV2 filter({tempConfig, gmConfig, brummConfig});
#else
DynamicAdaptiveFilterV2 filter({tempConfig, gmConfig});
#endif

// Interrupt für Geiger-Zähler
void IRAM_ATTR onGeigerPulse() {
  filter.onPulse(1); // Kanal 1 (gmConfig)
}

void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT); // Temperatur (z.B. Thermistor)
  pinMode(4, INPUT_PULLUP); // Geiger-Zähler (z.B. GPIO4)
  attachInterrupt(digitalPinToInterrupt(4), onGeigerPulse, FALLING);
  Serial.println("Filter gestartet: Temperatur (EMA), Geiger-Zähler (COUNT_MODE)");
}

void loop() {
  // Temperatur lesen (Kanal 0)
  float temp = analogRead(A0) * (3.3f / 4095.0f) * 100.0f; // Beispiel: Spannung -> °C
  SensorData data = {{temp, 0.0f}, {}, millis(), "TEMP_GM"};
  if (filter.pushSensorData(data)) {
    auto values = filter.getFilteredValues();
    Serial.printf("Temp: %.1f°C (Filtered: %.1f°C)\n", temp, values[0]);
  }

  // CPM vom Geiger-Zähler (Kanal 1)
  unsigned long cpm = filter.getCPM(1);
  if (cpm > 0) {
    auto values = filter.getFilteredValues();
    Serial.printf("CPM: %lu (Filtered CPS: %.1f)\n", cpm, values[1]);
  }

  // Optional: GPS (Kanal 2, Kalman)
#if defined(USE_KALMAN)
  float speed = 50.0f + random(-500, 500) / 100.0f; // Simulierte GPS-Daten
  SensorData gpsData = {{temp, 0.0f, speed}, {}, millis(), "GPS"};
  if (filter.pushSensorData(gpsData)) {
    auto values = filter.getFilteredValues();
    Serial.printf("Speed: %.1fkm/h (Filtered: %.1fkm/h)\n", speed, values[2]);
  }
#endif

  // Optional: Brummfilter (Kanal 2, LMS)
#if defined(USE_LMS)
  float adc = analogRead(A0); // Beispiel: Brummen auf A0
  float value = (adc / 4095.0f) * 100.0f;
  SensorData brummData = {{temp, 0.0f, value}, {0.0f, 0.0f, value}, millis(), "BRUMM"};
  if (filter.pushSensorData(brummData)) {
    auto values = filter.getFilteredValues();
    Serial.printf("Raw: %.1f%% (Filtered: %.1f%%)\n", value, values[2]);
  }
#endif

  delay(1000); // 1 Hz für Temperatur
}
```

---

## 📖 Projektstruktur

```
DynamicAdaptiveFilterV2/
├── DynamicAdaptiveFilterV2.cpp        # Hauptimplementierung
├── DynamicAdaptiveFilterV2.h          # Header-Datei
├── README.md                          # Hauptdokumentation
├── filter/                             # Filter
│   ├── FIR_coefficients.h              # Vordefinierte FIR-Koeffizienten
│   ├── FILTER.md                       # Detaillierte Filterbeschreibung
│   ├── FILTERTYPES.md                  # Theorie der Filtertypen
│   └── FILTERCOEFFS.md                 # Theorie der FIR-Koeffizienten
├── params/                             # Vordefinierte Parameter
│   ├── params_GMCT.h                   # Parameter für Geiger-Müller-Zählrohre
│   ├── params_analog.h                 # Parameter für ADC-Anwendungen
│   ├── params_sensors.h                # Parameter für gängige Sensoren
│   └── PARAMS.md                       # Beschreibung der Alltagsszenarien
└── examples/                           # Beispiel-Sketches

```

---

## 🧾 Lizenz

Diese Bibliothek steht unter der **MIT-Lizenz**
Frei nutzbar für **private und kommerzielle Projekte**.

---

