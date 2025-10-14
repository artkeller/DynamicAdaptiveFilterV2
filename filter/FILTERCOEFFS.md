# FILTERCOEFFS.md — Vordefinierte FIR-Koeffizienten für DynamicAdaptiveFilterV2

**Ziel:** Diese Datei erklärt die vordefinierten Filterkoeffizienten, die in `filter_coefficients.h` enthalten sind. Sie hilft Entwicklern, den passenden Filtertyp auszuwählen, ohne sich tief in digitale Signalverarbeitung (DSP) einarbeiten zu müssen.

Die Koeffizienten werden für **FIR-Filter** genutzt. Sie bestimmen, wie ein FIR-Filter eingehende Sensordaten glättet oder bestimmte Frequenzen unterdrückt.

---

## Grundprinzip: FIR-Koeffizienten

Ein **FIR-Filter (Finite Impulse Response)** berechnet seinen Ausgangswert als gewichtete Summe der letzten N Eingangswerte:

$y[n] = \sum_{k=0}^{N-1} b_k \cdot x[n-k]$

* $x[n]$: aktuelle und vergangene Messwerte (Sensorwerte)
* $b_k$: Filterkoeffizienten (aus dieser Datei)
* $N$: Anzahl der Taps (Filterordnung)

Die Form der Koeffizienten bestimmt die Art des Filters, z. B. Tiefpass, Hochpass oder Notch-Filter.

**Einfach erklärt:** Die Koeffizienten legen fest, wie stark alte Messwerte berücksichtigt werden. Bei einem symmetrischen Tiefpass werden ältere Werte gleichmäßig einbezogen, um Rauschen zu reduzieren.

---

## Struktur der Datei

Die Datei `filter_coefficients.h` enthält fertige Arrays mit typischen Filtertypen:

1. **Chebyshev Low-Pass** (Rauschen glätten, schnelle Änderungen sichtbar halten)
2. **Bessel Low-Pass** (wenig Phasenverzerrung, ideal für Bewegungssensoren)
3. **Butterworth Low-Pass** (maximale Glättung ohne Wellen in der Frequenzantwort)
4. **Notch-Filter für 50 Hz** (speziell zur Netzbrumm-Unterdrückung)

Für die ersten drei Filtertypen gibt es jeweils drei Ordnungen:

* **Ordnung 1 (3 Taps):** sehr einfach, reagiert schnell
* **Ordnung 2 (5 Taps):** mehr Glättung
* **Ordnung 3 (7 Taps):** sehr glatte Signale, etwas mehr Verzögerung

---

## 1) Chebyshev Low-Pass

**Einsatzgebiet:** Sensoren mit schwankendem Rauschen, wo schnelle Signaländerungen erhalten bleiben sollen.

* **Merkmale:**

  * Lässt Nutzsignale durch, dämpft aber Rauschen.
  * Hat eine leichte Welligkeit („Ripple“) im Durchlassbereich (hier 0.5 dB).
  * Ideal, wenn ein guter Kompromiss zwischen Reaktionsgeschwindigkeit und Glättung gesucht wird.

**Praktische Beispiele:**

* Temperatur, die auf schnelle Änderungen reagieren soll
* Luftqualitätssensoren, bei denen Peaks wichtig sind
* GPS-Höhenfilter, wenn nur wenig Verzögerung erlaubt ist

**Koeffizienten:**

| Ordnung | Taps | Arrayname                  |
| ------- | ---- | -------------------------- |
| 1       | 3    | `chebyshev_lowpass_order1` |
| 2       | 5    | `chebyshev_lowpass_order2` |
| 3       | 7    | `chebyshev_lowpass_order3` |

---

## 2) Bessel Low-Pass

**Einsatzgebiet:** IMUs, Robotik, Steuerungen — überall dort, wo die **Signalform und Zeitrelation** wichtig sind.

* **Merkmale:**

  * Minimal mögliche Phasenverzerrung → Bewegungssignale bleiben zeitlich korrekt.
  * Gute Wahl für **Echtzeit-Steuerungen** oder Fusion mehrerer Sensoren.
  * Perfekt für Anwendungen, die nicht nur glätten, sondern auch auf Timing angewiesen sind.

**Praktische Beispiele:**

* Drohnenflugsteuerung (Gyroskop, Beschleunigung)
* VR-Controller (Bewegungsdaten)
* Robotik (Motorregelung mit Feedback)

**Koeffizienten:**

| Ordnung | Taps | Arrayname               |
| ------- | ---- | ----------------------- |
| 1       | 3    | `bessel_lowpass_order1` |
| 2       | 5    | `bessel_lowpass_order2` |
| 3       | 7    | `bessel_lowpass_order3` |

---

## 3) Butterworth Low-Pass

**Einsatzgebiet:** Starke Rauschunterdrückung mit **maximal glatter Frequenzantwort**.

* **Merkmale:**

  * Keine Welligkeit im Durchlassbereich.
  * Sehr starke Glättung, reagiert etwas träger auf plötzliche Änderungen.
  * Gute Wahl, wenn Messwerte stabil angezeigt werden sollen.

**Praktische Beispiele:**

* Anzeige von Umweltwerten (Temperatur, Luftdruck, Feinstaub)
* Datenlogging, bei dem Sprünge nicht gewünscht sind
* Sanfte Glättung für visuelle Darstellung

**Koeffizienten:**

| Ordnung | Taps | Arrayname                    |
| ------- | ---- | ---------------------------- |
| 1       | 3    | `butterworth_lowpass_order1` |
| 2       | 5    | `butterworth_lowpass_order2` |
| 3       | 7    | `butterworth_lowpass_order3` |

---

## 4) Notch-Filter für 50 Hz Brummen

**Einsatzgebiet:** Elektrische Störungen wie Netzbrummen (50 Hz in Europa, 60 Hz in USA).

* **Merkmale:**

  * Speziell dafür entworfen, genau 50 Hz zu unterdrücken.
  * Andere Frequenzen werden weitgehend unbeeinflusst gelassen.
  * Nur 5 Taps → geringer Rechenaufwand.

**Praktische Beispiele:**

* Mikrofone / Audiosignale, die Brummen enthalten
* Analoge ADC-Signale mit Störspannungen aus Netzteilen
* Präzisionssensoren in Laborumgebungen

**Koeffizientenarray:** `notch_50hz`

Beispiel FIR-Definition im Code:

```cpp
{ FIR, 0, notch_50hz, 5, 100.0f, 10000, 1000, 5.0f, 0.0f, VALUE_MODE }
```

---

## Auswahlhilfe für den richtigen Filter

| Ziel                                         | Empfohlener Filtertyp | Empfohlene Ordnung |
| -------------------------------------------- | --------------------- | ------------------ |
| Starke Rauschunterdrückung, visuelle Anzeige | **Butterworth**       | 2 oder 3           |
| Bewegungssensoren, Echtzeitsteuerung         | **Bessel**            | 2                  |
| Allgemeiner Sensor mit schwankendem Rauschen | **Chebyshev**         | 1 oder 2           |
| 50 Hz Netzbrumm entfernen                    | **Notch 50 Hz**       | 5 Taps             |

> **Hinweis:** Höhere Ordnung bedeutet stärkere Glättung, aber auch mehr Verzögerung (Latenz) und etwas mehr Speicherverbrauch.

---

## Anwendung in der DynamicAdaptiveFilterV2

Um einen FIR-Filter mit vordefinierten Koeffizienten zu nutzen, muss nur das passende Array angegeben werden:

Beispiel: Bessel Low-Pass für eine IMU-Achse

```cpp
#include "filter_coefficients.h"
#include "DynamicAdaptiveFilterV2.h"

FilterConfig imu_filter = {
  FIR,              // Filtertyp
  0,                // Länge irrelevant für FIR
  bessel_lowpass_order2, // vordefinierte Koeffizienten
  5,                // Anzahl der Taps
  100.0f,           // Abtastrate (Hz)
  1000,             // maxDecayTimeMs
  1000,             // warmUpTimeMs
  5.0f,             // ThresholdPercent
  0.0f,             // DeadTimeUs (nur COUNT_MODE)
  VALUE_MODE        // kontinuierlicher Wert
};
```

---

## Praktischer Nutzen

* Die Datei `filter_coefficients.h` liefert fertige, getestete FIR-Koeffizienten.
* Sie erleichtert die Arbeit für Entwickler, die keinen eigenen Filterentwurf machen wollen.
* Auswahl hängt vom Anwendungsfall ab:

  * **Chebyshev:** Kompromiss zwischen Reaktionsgeschwindigkeit und Glättung.
  * **Bessel:** minimalste Verzerrung für Bewegungsdaten.
  * **Butterworth:** maximale Glättung für stabile Anzeigen.
  * **Notch:** gezielte Unterdrückung von 50 Hz Netzbrummen.

So könnwn sofort funktionierende Filter im Projekt eingesetz werden, ohne komplexe DSP-Berechnungen durchzuführen.
