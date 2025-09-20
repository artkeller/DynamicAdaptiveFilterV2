# DynamicAdaptiveFilterV2: Detaillierte Dokumentation

## Inhaltsverzeichnis

* [Was ist ein Filter?](#was-ist-ein-filter)
* [Filtertypen](#filtertypen)
* [Ressourcenverbrauch](#ressourcenverbrauch)
* [Konfiguration](#konfiguration)
* [Beispiel: Kalman für GPS](#beispiel-kalman-für-gps)
* [Beispiel: LMS für Brummfilter](#beispiel-lms-für-brummfilter)

---

## Was ist ein Filter?

Ein **Filter** glättet verrauschte Messwerte und bereitet sie für weitere Verarbeitung oder Visualisierung auf.
Die **DynamicAdaptiveFilterV2** Bibliothek unterstützt **Multi-Kanal-Filterung**, sodass mehrere Messgrößen gleichzeitig verarbeitet werden können.

**Beispiele:**

* Geiger-Zähler: **CPM** + **µSv/h** gleichzeitig
* GPS-Modul: Geschwindigkeit + Höhe
* IMU: Beschleunigung + Gyro + Magnetometer

---

## Filtertypen

| Filtertyp                            | Beschreibung                                                                                       | Typischer Einsatz                |
| ------------------------------------ | -------------------------------------------------------------------------------------------------- | -------------------------------- |
| **EMA** (Exponential Moving Average) | Exponentieller gleitender Durchschnitt, sehr schnell und speichersparend                           | Temperatur, Luftfeuchtigkeit     |
| **SMA** (Simple Moving Average)      | Einfacher gleitender Durchschnitt, ideal für langsame Signale                                      | Drucksensoren, Feinstaubsensoren |
| **FIR** (Finite Impulse Response)    | Präziser Filter, gut geeignet für Frequenzfilterung wie 50 Hz Brummunterdrückung                   | Audiosignale, IMU                |
| **Kalman**                           | Modellbasierter Filter, kombiniert Vorhersagen und Messungen. Aktivieren mit `#define USE_KALMAN`. | GPS, IMU, Robotik                |
| **LMS** (Least Mean Squares)         | Adaptiver FIR-Filter, passt sich dynamisch an. Aktivieren mit `#define USE_LMS`.                   | Rauschunterdrückung, Brummfilter |
| **RLS** (Recursive Least Squares)    | Sehr schnell konvergierender adaptiver FIR-Filter. Aktivieren mit `#define USE_RLS`.               | Hochpräzise Sensorfusion         |

> **Hinweis:**
> Aus Performancegründen kann nur **einer der adaptiven Filtertypen (Kalman, LMS, RLS)** gleichzeitig aktiviert werden.

---

## Ressourcenverbrauch

| Filtertyp  | RAM/Kanal   | CPU (10 Hz) | Flash  |
| ---------- | ----------- | ----------- | ------ |
| **EMA**    | \~50 Bytes  | < 0.1 ms    | \~1 KB |
| **SMA**    | \~100 Bytes | \~0.1 ms    | \~1 KB |
| **FIR**    | \~100 Bytes | \~0.1 ms    | \~2 KB |
| **Kalman** | \~100 Bytes | \~0.5 ms    | \~3 KB |
| **LMS**    | \~100 Bytes | \~0.1 ms    | \~2 KB |
| **RLS**    | \~200 Bytes | \~0.5 ms    | \~4 KB |

> Diese Werte gelten pro Kanal und bei einer Abtastrate von 10 Hz.
> Auf einem ESP32 sind selbst mehrere Dutzend Kanäle problemlos möglich.

---

## Konfiguration

Um **erweiterte Filtertypen** wie Kalman, LMS oder RLS zu aktivieren, müssen entsprechende Makros **vor** dem `#include` der Bibliothek definiert werden.

```cpp
#define USE_KALMAN   // Aktiviert Kalman-Filter
#define USE_LMS      // Aktiviert LMS-Filter
#define USE_RLS      // Aktiviert RLS-Filter

#include "DynamicAdaptiveFilterV2.h"
```

> **Achtung:**
> Nur **einer** dieser Makros darf gleichzeitig aktiv sein, z. B. `USE_KALMAN` **oder** `USE_LMS`.

---

## Beispiel: Kalman für GPS

Dieses Beispiel zeigt die Filterung von **Geschwindigkeit** und **Höhe** eines GPS-Moduls mithilfe eines Kalman-Filters.
Der Kalman-Filter reduziert GPS-Rauschen und liefert glatte, stabile Werte.

```cpp
#define USE_KALMAN
#include <TinyGPSPlus.h>
#include "DynamicAdaptiveFilterV2.h"

TinyGPSPlus gps;

// Zwei Kanäle: Geschwindigkeit und Höhe
std::vector<FilterConfig> filter_gps = {
  {KALMAN, 0, nullptr, 0, 10.0f, 10000, 1000, 5.0f, 100.0f, VALUE_MODE, 0.01f, 0.1f, 0.0f}, // Geschwindigkeit
  {KALMAN, 0, nullptr, 0, 10.0f, 10000, 1000, 5.0f, 100.0f, VALUE_MODE, 0.01f, 0.1f, 0.0f}  // Höhe
};

DynamicAdaptiveFilterV2 filter(filter_gps);

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("GPS-Filter gestartet...");
}

void loop() {
  while (Serial1.available()) {
    gps.encode(Serial1.read());
  }

  if (gps.location.isUpdated()) {
    float speed = gps.speed.kmph();
    float altitude = gps.altitude.meters();

    SensorData data = { {speed, altitude}, millis(), "GPS" };

    filter.pushSensorData(data);
    auto filtered = filter.getFilteredValues();

    Serial.printf("Speed: %.1f km/h (Filtered: %.1f km/h), Altitude: %.1f m (Filtered: %.1f m)\n",
                  speed, filtered[0], altitude, filtered[1]);
  }

  delay(100);
}
```

---

## Beispiel: LMS für Brummfilter

Ein LMS-Filter eignet sich hervorragend, um **periodisches Rauschen** wie Netzbrummen (50 Hz) aus analogen Signalen zu entfernen.

```cpp
#define USE_LMS
#include "DynamicAdaptiveFilterV2.h"

// LMS-Filter mit Fenstergröße 10
std::vector<FilterConfig> filter_brumm = {
  {LMS, 10, nullptr, 0, 100.0f, 10000, 1000, 5.0f, 100.0f, VALUE_MODE, 0.01f, 0.0f}
};

DynamicAdaptiveFilterV2 filter(filter_brumm);

void setup() {
  Serial.begin(115200);
  pinMode(19, INPUT);
  Serial.println("Brummfilter gestartet...");
}

void loop() {
  int adc = analogRead(19);
  float value = (adc / 4095.0f) * 100.0f;

  SensorData data = { {value}, millis(), "BRUMM" };

  filter.pushSensorData(data);
  auto filtered = filter.getFilteredValues();

  Serial.printf("Raw: %.1f%%, Filtered: %.1f%%\n", value, filtered[0]);
  delay(10);
}
```

> **Tipp:**
>
> * Die Lernrate `mu` im `FilterConfig` beeinflusst, wie schnell sich der Filter anpasst.
> * Für Brummunterdrückung typischerweise Werte zwischen **0.001** und **0.01** verwenden.

---

## Zusammenfassung

Die **DynamicAdaptiveFilterV2** Bibliothek bietet:

* Multi-Kanal-Sensorfilterung in Echtzeit
* Unterstützung einfacher und adaptiver Filtertypen
* Optimierten Ressourcenverbrauch
* Einfache Konfiguration und klare Beispiele für den direkten Einsatz

Sie ist damit ideal für Anwendungen wie:

* GPS-Tracking
* Robotik und Drohnen
* Umweltmessungen
* Geiger-Müller-Zähler
* Audiosignalverarbeitung
