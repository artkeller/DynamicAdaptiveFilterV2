# FILTERTYPES.md — Theorie und Praxis der Filtertypen in DynamicAdaptiveFilterV2

**Zielgruppe:** Entwickler mit grundlegenden Kenntnissen in Embedded-C++/Arduino; keine tiefgehende DSP-Vorkenntnis erforderlich.

Diese Datei erklärt ausführlich die Filtertypen, die **DynamicAdaptiveFilterV2** unterstützt, ihre mathematische Basis, praktische Auswirkungen und wie die `FilterConfig`-Parameter das Verhalten steuern. Fokus liegt auf konkreten Einsatzszenarien (Sensoren, Sound, GPS, IMU, Geiger-Müller-Zählrohr u.ä.).

---

## Kurzübersicht (Was wird behandelt)

* EMA (Exponential Moving Average)
* SMA (Simple Moving Average)
* FIR (Finite Impulse Response)
* Kalman-Filter (modellbasiert)
* LMS (Least Mean Squares, adaptiv)
* RLS (Recursive Least Squares, adaptiv)

Für jeden Typ: mathematische Grundlage, Implementationshinweise, Vor-/Nachteile, praktische Parameterempfehlungen.

---

## 0. Wichtige Konzepte (Grundbegriffe)

* **Abtastrate / normale Frequenz (`normalFreqHz`)**: Erwartete Messrate; wird in der Bibliothek verwendet, um `expectedIntervalMs = 1000 / normalFreqHz` zu berechnen. Hilft beim Umgang mit unregelmäßigen Messintervallen.
* **Warm-up (`warmUpTimeMs`)**: Zeit, in der der Filter zuerst Werte sammelt und initialisiert—wichtig, damit Mittelwerte und History nicht mit Nullen oder zufälligen Werten starten.
* **Decay / Alterung (`maxDecayTimeMs`)**: Wenn Messungen längere Zeit ausbleiben, reduziert sich der Einfluss vergangener Samples (Decay-Faktor). So verhält sich der Filter robust bei sporadischen Messungen.
* **Threshold (`thresholdPercent`)**: Schwellwert (%) für "signifikante" Änderungen; kleine Abweichungen können ignoriert werden, um unnötige Updates zu vermeiden.
* **COUNT\_MODE & Dead Time**: Für Impulszähler (z. B. Geiger-Müller). Dead time (`deadTimeUs`) schützt vor Doppelzählungen und wird bei Korrektur der Rate berücksichtigt.

---

## 1) EMA — Exponentieller gleitender Durchschnitt

**Kurz:** Ein speicherschonender, gewichteter Filter; neuere Werte erhalten exponentiell mehr Gewicht.

**Formel (diskret):**

$y_n = \alpha \cdot x_n + (1 - \alpha) \cdot y_{n-1}$

wobei $0 < \alpha \le 1$ die Glättungs-Konstante ist.

**In der Bibliothek:**

* `baseAlpha` wird aus `length` berechnet als `2/(length+1)` (klassische N→α-Umrechnung). Wenn `length` = 1 → `alpha` = 1 (keine Glättung).
* Zusätzlich gibt es einen **Decay-Faktor** (abhängig von `deltaT` gegenüber `expectedIntervalMs` und `maxDecayTimeMs`). Die Bibliothek berechnet einen `effectiveAlpha`:

  * `effectiveAlpha = 1.0f - decayFactor * (1.0f - baseAlpha)`
  * Wenn `decayFactor == 1` → `effectiveAlpha == baseAlpha` (normales Verhalten).
  * Wenn `decayFactor == 0` (sehr lange Pause) → `effectiveAlpha == 1` (neuer Messwert übernimmt vollständig → Reset-Effekt).

**Wirkung/Charakteristik:**

* Sehr niedriger Speicherbedarf (nur letzter Ausgabewert nötig).
* Reagiert schnell auf plötzliche Änderungen, aber abhängig von `alpha` kann es Nachlauf/Trägheit geben.
* Nützlich, wenn Messung ohne großen Rechenaufwand geglättet werden soll.

**Einsatzbeispiele & Tuning:**

* Temperatur / Feuchte: `length` 5–20 (alpha \~ 0.09–0.33) → sanfte Glättung.
* Geiger-Müller-CPM (nach Zählung): EMA mit kurzem `length` (= 3–10) um kurze Schwankungen zu glätten, kombiniert mit Dead-Time-Korrektur.
* GPS-Höhe / Barometer (slow-changing): moderate `length` 10–30.

**Vor-/Nachteile:**

* Einfach, wenig Speicher
* Einfach zu konfigurieren (über `length`)
* Kein linearer Phasengang (bei manchen Systemen ohne Bedeutung)
* Kann kurzfristige periodische Störungen nicht gezielt herausfiltern (keine Notch/Brumm-Unterdrückung)

---

## 2) SMA — Simple (rolling) Moving Average

**Kurz:** Durchschnitt über ein festes Fenster gleich gewichteter Werte.

**Formel:**

$y_n = \frac{1}{N} \sum_{k=0}^{N-1} x_{n-k}$

**Implementierung in der Bibliothek:**

* `baseCoeffs` wird initialisiert mit `1/N` für alle `N` Einträge.
* `history` speichert die letzten Messwerte (Länge N).
* Beim Update werden ältere Koeffizienten bei Bedarf durch einen `decayFactor` skaliert (für unregelmäßige Eingaben).

**Wirkung/Charakteristik:**

* Glättet stark, aber führt Verzögerung (Latenz) von \~N/2 Samples ein.
* Einfach zu verstehen. Gut für langsame Signale ohne hohe Frequenzkomponenten.

**Einsatz & Tuning:**

* Luftdruck, PM-Sensoren: `N = 5..20`.
* PM-Werte: `N = 10` ist in vielen Anwendungen ein guter Kompromiss zwischen Glättung und Reaktionszeit.

**Vor-/Nachteile:**

* Stabil, deterministisch
* Kein Tuning mit Lernraten nötig
* Verzögerung proportional zu Fenstergröße
* Nicht geeignet zur selektiven Unterdrückung einer bestimmten Frequenz (z. B. 50Hz-Brumm)

---

## 3) FIR — Finite Impulse Response

**Kurz:** Linearer Filter, dessen Ausgang die gewichtete Summe einer endlichen Anzahl vergangener Eingaben ist. Sehr flexibel — durch geeignete Koeffizienten können Tiefpass-, Hochpass-, Bandpass- oder Notch-Filter realisiert werden.

**Formel:**

$y[n] = \sum_{k=0}^{M-1} b_k \cdot x[n-k]$

wobei `b_k` die Filterkoeffizienten sind und `M = numCoeffs`.

**Implementierung in Bibliothek:**

* `baseCoeffs` hält die FIR-Koeffizienten (z.B. Chebyshev, Bessel, benutzerdefiniert).
* `history` ein Ringpuffer der letzten `M` Werte.
* Vor dem Einsatz prüft die Bibliothek, ob die Koeffizienten ungefähr auf 1 summieren (Warnung, falls nicht): ein normalisierter Summe → DC-Gain ≈ 1.
* Bei unregelmäßigen Einträgen werden vergangene Koeffizienten mit `decayFactor` skaliert.

**Eigenschaften:**

* FIRs können linearphasig gemacht werden (symmetrische b\_k), was für IMU/Steuerungsanwendungen wichtig ist.
* Mehr Koeffizienten → schärferer Übergang (steilere Filterkurve) → aber mehr Speicher und Latenz.

**Praxisbeispiele & Empfehlungen:**

* IMU (Beschleunigung/Gyroskop): **Bessel-FIR** (oder linearphasige FIR) wegen geringer Phasenverzerrung. `numCoeffs = 5..31` abhängig von gewünschtem Cutoff und Samplingrate.
* Audio / Brummunterdrückung: Notch FIR oder speziell entworfene Koeffizienten (Chebyshev für steile Roll-off). Für 50/60 Hz-Notch: Entwurf mit genügend Taps (z. B. 31–101) oder adaptiv (siehe LMS).
* GPS (Positionsglättung): kurzes FIR für Lat/Lon kann helfen, schwankende Messungen zu glätten.

**Vor-/Nachteile:**

* Vorhersehbares Verhalten, kein Stabilitätsproblem wie bei IIR
* Linearphasige Varianten möglich
* Rechen- und Speicheraufwand steigt mit Anzahl Koeffizienten
* Latenz (wichtig bei Steuerungs-Anwendungen)

---

## 4) Kalman-Filter

**Kurz:** Ein rekursiver, modellbasierter Filter, der Schätzungen (Zustände) kombiniert aus:

* **Vorhersage** mittels eines Systemmodells, und
* **Messkorrektur** mittels realer Messungen.

Kalman ist optimal (im quadratischen Sinn) für lineare Systeme mit Gaußschem Rauschen.

**Grundgleichungen (vereinfacht, 1D):**

* Vorhersage: $x_{k|k-1} = A x_{k-1|k-1}$
* Kovarianz vorhersage: $P_{k|k-1} = A P_{k-1|k-1} A^T + Q$
* Kalman-Gewicht: $K_k = P_{k|k-1} (P_{k|k-1} + R)^{-1}$
* Update Zustand: $x_{k|k} = x_{k|k-1} + K_k (z_k - x_{k|k-1})$
* Update Kovarianz: $P_{k|k} = (I - K_k) P_{k|k-1}$

**Parameter:**

* `Q` (Prozessrauschen): Unsicherheit im Modell (größer → Filter vertraut Vorhersage weniger).
* `R` (Messrauschen): Unsicherheit der Messung (größer → Filter vertraut Messung weniger).
* `initialState`: Anfangswert des Zustands (und `P` initialisiert in Code).

**In der Bibliothek:**

* Kleine, einfache Kalman-Implementierung (1D pro Kanal) ist optional (nur wenn `USE_KALMAN` definiert).
* R wird adaptiv leicht angepasst in Beispielcode (`config.R = 0.99f * config.R + 0.01f * error*error`), um Messrauschen realitätsnäher zu verfolgen.

**Einsatz & Tuning:**

* **GPS + IMU Fusion:** Kalman ist Ideal, um Messungen (GPS: langsam, verrauscht) mit Modell-Vorhersagen (IMU: schnell, driften) zu kombinieren.

  * Setze `Q` klein (stabile Modellannahme) wenn Bewegung konsistent ist.
  * Erhöhe `R` wenn Messungen sehr verrauscht sind.
* **Höhe, Geschwindigkeit:** Kalman reduziert Sprünge und Schaltartefakte in GPS-Daten.

**Vor-/Nachteile:**

* Sehr leistungsfähig bei kombinierter Modell-/Messdatenfusion
* Geringe Verzögerung gegenüber gleitenden Mittelwerten
* Modellabhängig: schlechtes Modell → schlechte Schätzung
* Mathematisch anspruchsvoller (State-Transition, Multi-Dimension erfordert Matrizen)

---

## 5) LMS — Least Mean Squares (adaptives FIR)

**Kurz:** Ein adaptiver Algorithmus, der Koeffizienten eines FIR-Filters iterativ so anpasst, dass der Mittelwert des quadratischen Fehlers minimiert wird.

**Update-Regel (Vektorform):**

$\mathbf{w}_{k+1} = \mathbf{w}_k + \mu \cdot e_k \cdot \mathbf{x}_k$

wobei `e_k = d_k - y_k` (Fehler zwischen gewünschtem Ziel `d_k` und aktuellem Filter-Output `y_k`), `x_k` ist Input-Vektor und `mu` die Lernrate.

**In der Bibliothek:**

* `coeffs[]` und `inputBuffer[]` werden benutzt; `bufferIndex` rotiert über die Eingaben.
* `mu` kommt aus `FilterConfig` und steuert Konvergenzgeschwindigkeit/stabilität.

**Anforderungen:**

* LMS braucht eine **Fehler-/Referenzquelle** `d_k`, oder eine selbstadaptive Struktur (z. B. "predict-and-cancel"), sonst lernt es auf das Signal selbst.

**Einsatz & Tuning:**

* **Brummfilter (50/60 Hz):** LMS kann auf einen Referenzton oder auf das erwartete Sinusmuster trainiert werden, um genau diese Komponente zu entfernen.
* **Mikrofon-Rauschunterdrückung:** in Kombination mit Sekundärmikrofon (Referenzrauschen).
* `mu` typische Werte: `0.001 .. 0.01` (kleiner → stabil aber langsam; größer → schnell, evtl. instabil).

**Vor-/Nachteile:**

* Passt sich an veränderliche Störprofile an
* Einfach zu implementieren
* Braucht Referenz/Fehlerberechnung oder kluge Struktur
* Kann langsam konvergieren (abhängig von `mu`)

---

## 6) RLS — Recursive Least Squares

**Kurz:** Ein adaptiver Algorithmus wie LMS, aber mit wesentlich schnellerer Konvergenz und höherer Rechenkomplexität.

**Kernidee:** Minimiert rekursiv die gewichtete Summe quadratischer Fehler mit einem Forgetting-Factor `\lambda`.

**Parameter:**

* `lambda` (forgetting factor): typischerweise `0.98 .. 0.999`; näher an 1 → langsameres Vergessen → stabiler bei stationären Signalen.

**In der Bibliothek:**

* `coeffs[]`, `inputBuffer[]`, `P` Kovarianz werden verwaltet. `P` initialisiert mit 1.0.

**Einsatz & Tuning:**

* Wenn schnelle Anpassung an veränderte Störsignale erforderlich ist (z. B. wechselnde Frequenzen), ist RLS besser als LMS.
* RLS ist rechenintensiver (Matrix/Vektor-Operationen intern) → CPU & RAM beachten.

**Vor-/Nachteile:**

* Sehr schnelle Konvergenz
* Besser für nicht-stationäre Umgebungen
* Höherer Rechenaufwand und numerische Stabilitätsfragen

---

## FilterConfig: Feld-für-Feld-Erklärung und Wirkung

Die Struktur `FilterConfig` (Reihenfolge wie im Header) steuert das Verhalten jedes Kanals. Hier jede Komponente mit Bedeutung und Praxiswerten:

```cpp
struct FilterConfig {
  FilterType type;        // EMA, SMA, FIR, (KALMAN), (LMS), (RLS)
  int length;             // Für EMA/SMA: Fensterlänge. Für adaptive Filter: Anzahl Koeffizienten.
  const float* coeffs;    // Für FIR: Pointer auf Koeffizienten-Array (oder nullptr)
  int numCoeffs;          // Für FIR: Länge des coeffs-Arrays
  float normalFreqHz;     // Erwartete Messfrequenz in Hz (z. B. 10.0f)
  unsigned long maxDecayTimeMs; // Max. Zeit bis Decay == 0 (ms)
  unsigned long warmUpTimeMs;   // Zeit zum Initialisieren (ms)
  float thresholdPercent; // Minimale relative Änderung (%) bevor ein Update als signifikant gilt
  float deadTimeUs;       // Totzeit in µs (nur COUNT_MODE)
  FilterMode mode;        // VALUE_MODE oder COUNT_MODE
  // Optional: Kalman/LMS/RLS Parameter folgen (nur wenn Makros gesetzt)
};
```

### `type`

* Wählt die Filterklasse. Bestimme zuerst den Anwendungstyp (impulsbasiert vs. kontinuierlich, adaptiv vs. statisch).

### `length`

* **EMA:** Interpretiert als "Fenstergröße" N, von der `baseAlpha = 2/(N+1)` abgeleitet wird. Größeres `length` → kleinere `alpha` → stärkere Glättung.
* **SMA:** Anzahl Samples im Fenster. Größer → mehr Glättung, mehr Latenz.
* **LMS/RLS:** Anzahl Koeffizienten bzw. Filterordnung. Größer → feinere Filtercharakteristik möglich, mehr Parameter zu lernen.

**Praxis:** Für Temperatur z. B. `length = 10`; für Audio-Brummfilter mit LMS evtl. `length = 32`.

### `coeffs` / `numCoeffs` (nur FIR)

* `coeffs` enthält die FIR-Taps. Diese müssen normalerweise so skaliert sein, dass ihre Summe ≈ 1.0 (für DC-Gain=1).
* Wenn Summe deutlich ≠ 1, macht die Bibliothek eine Warnung, aber die Benutzung ist trotzdem möglich (bewusst anderes DC-Gain-Verhalten).

**Hinweis:** Verwende fertige Entwurfstools (Python/Matlab/Octave) oder vorbereitete Arrays (z. B. `chebyshev_lowpass`, `bessel_lowpass`) für gängige Filtercharakteristiken.

### `normalFreqHz`

* Heuristisch verwendet, um `expectedIntervalMs` zu berechnen. Wichtig für die interne Decay-Berechnung und robusten Umgang bei unregelmäßigen Messungen.

### `maxDecayTimeMs`

* Zeit, nach der ein Messintervall als "zu alt" gilt und vergangene Koeffizienten vollständig entwertet werden (decayFactor→0). Setze dies so, dass es größer ist als typische Kommunikationsausfälle, aber nicht so groß, dass veraltete Werte ewig wirken.

### `warmUpTimeMs`

* Insbesondere für SMA/FIR wichtig (damit `history` sinnvoll initialisiert wird). Bei COUNT\_MODE wird auf mindestens 60s gesetzt, da CPM-Berechnung über 60s sinnvoll ist.

### `thresholdPercent`

* Verhindert unnötige Updates: Wenn relative Änderung kleiner als `thresholdPercent`, wird Wert ignoriert. Nützlich z. B. für GPS-Satellitenanzahl (nur bei größeren Sprüngen melden).

### `deadTimeUs` (COUNT\_MODE)

* Totzeit in Mikrosekunden; wird genutzt, um Doppelzählungen zu vermeiden und für Totzeitkorrektur bei CPM/CPS.
* Typische GM-Zählrohr-Werte: 100..1000 µs abhängig vom Zählrohr.

### `mode`

* `VALUE_MODE`: kontinuierliche Werte (Temperatur, ADC, IMU).
* `COUNT_MODE`: Impulszählung (z. B. Geiger-Müller). Impulse werden per `onPulse()` erfasst; `getCPM()` berechnet CPM und führt Totzeitkorrektur durch.

### Optionale Felder (nur bei Makros aktiviert)

* **Kalman:** `Q`, `R`, `initialState` — Prozess- und Messrauschen + Anfangsschätzung.
* **LMS:** `mu` — Lernrate.
* **RLS:** `lambda` — Forgetting-Faktor.

---

## Specials: Decay-Faktor, unregelmäßige Abtastraten

Die Bibliothek berücksichtigt unregelmäßige Abtastraten durch `calculateDecayFactor(state, deltaT)`:

* `deltaT <= expectedIntervalMs` → `decayFactor = 1.0` (normales Verhalten)
* `deltaT >= maxDecayTimeMs` → `decayFactor = 0.0` (alter Einflüsse werden verworfen)
* sonst linear interpoliert zwischen 1 und 0.

**Auswirkung:**

* Bei langen Pausen wird der Filter responsiver auf den nächsten Messwert. Das verhindert extrem träges Verhalten nach Kommunikationsausfällen.
* Für FIR/SMA: Vergangene Koeffizienten werden skaliert und der Restgewich auf das neueste Sample umgeleitet, so bleibt die Filterausgabe normalisiert.

---

## Latenz & Phasenverhalten — Warum das wichtig ist

* **SMA / FIR mit vielen Taps** → merkliche Latenz ≈ (M-1)/2 Samples (M = Taps), kritisch für Regelungen/Control.
* **Linearphasige FIRs** (symmetrische Koeffizienten) haben konstante Verzögerung und sind bevorzugt bei IMU/Steuerung.
* **IIR/Kalman**: Nicht-linearer Phase, aber geringe Latenz und guter Laufzeitbetrieb.

**Regel:** Wenn Steuerkreis/Realtime wichtig → bevorzuge Kalman (modellbasiert) oder sehr kurze FIR/Taps. Wenn nur Anzeige/Logging wichtig → längere FIR/SMA/EMA ok.

---

## Ressourcen & Skalierung

* Jeder Kanal bringt eigenen Speicher- und CPU-Bedarf. FIR/LMS/RLS mit vielen Koeffizienten skalieren linear.
* Kalman erfordert geringe Matrix-Operationen pro Kanal (1D in dieser Implementierung). RLS ist rechenintensiv.

---

## Konkrete Tuning-Empfehlungen (Quick Reference)

* **Temperatur / Feuchte (SHTx, DS18B20):** EMA `length = 5..15`, `warmUpTimeMs = 2000`.
* **Druck / Barometer:** SMA `N = 5..10` oder FIR mit 5–11 taps.
* **Feinstaub (PMS5003 / SPS30):** SMA `N = 10`.
* **IMU (Acc / Gyro):** FIR (Bessel) mit `numCoeffs = 5..21`; Sampling 100..1000 Hz; kleines `maxDecayTimeMs` (z. B. 1000 ms).
* **GPS:** Kalman für Positions-/Geschwindigkeitsfusion; Q klein (1e-3..1e-1), R abhängig von Messunsicherheit (z. B. R \~ var(GPS position) m^2).
* **Audio / Brumm 50Hz:** FIR-Notch große Taps (31+), alternativ LMS mit `mu = 0.001..0.01` und `length=32`.
* **Geiger-Müller (COUNT\_MODE):** EMA für CPM glätten (`length = 5`), `deadTimeUs = 100..1000`, `warmUpTimeMs >= 60000` (60 s sinnvoll für CPM-Berechnung).

---

## Praktische Beispiele (FilterConfig-Snippets)

**EMA für Temperatur:**

```cpp
{ EMA, 10, nullptr, 0, 1.0f, 86400000, 2000, 2.0f, 0.0f, VALUE_MODE }
```

**FIR (Bessel) für IMU-Achse (5 Taps):**

```cpp
{ FIR, 0, bessel_lowpass, 5, 10.0f, 3600000, 1000, 5.0f, 0.0f, VALUE_MODE }
```

**Kalman für GPS-Geschwindigkeit (benötigt `USE_KALMAN`):**

```cpp
{ KALMAN, 0, nullptr, 0, 10.0f, 10000, 1000, 5.0f, 0.0f, VALUE_MODE, 0.01f, 0.1f, 0.0f }
// Q=0.01, R=0.1, initialState=0.0
```

**LMS für Brummfilter (benötigt `USE_LMS`):**

```cpp
{ LMS, 32, nullptr, 0, 100.0f, 10000, 1000, 5.0f, 0.0f, VALUE_MODE, 0.005f }
// mu=0.005
```

---

## Fallstricke & Hinweise

* **Nicht mehrere adaptive Filtertypen gleichzeitig kompilieren:** Header stellt preprocessor-Fehler sicher (nur `USE_KALMAN` **oder** `USE_LMS` **oder** `USE_RLS`).
* **Korrekte Initialisierung:** Bei FIR/SMA immer `warmUpTimeMs` beachten oder `initializeHistory()` verwenden, damit erste Ausgaben sinnvoll sind.
* **Normalization der FIR-Koeffizienten:** Prüfe, dass Sum(coeffs) ≈ 1, wenn du DC-Gain = 1 erwartest.
* **Dead-Time-Korrektur (GM):** Die im Code implementierte Korrektur teilt durch `(1 - cps * deadTime)` (approx.); bei sehr hohen Raten kann Korrektur numerisch unsicher werden.
* **Maximale Anzahl adaptive Koeffizienten:** LMS/RLS benutzen statische Arrays `MAX_FILTER_LENGTH` — definiere diesen Wert passend.

---

## Weiteres Vorgehen

* Visualisiere Eingangssignal, gefiltertes Signal und Fehler (z. B. in Python/Matplotlib) um Parameter zu verfeinern.
* Bei unsicherer Wahl: starte mit EMA (einfach) und messe Performance/Qualität; wechsele zu FIR/Kalman/LMS wenn spektrale/Modellanforderungen dies verlangen.

---

## Quellen & weiterführende Literatur (empfohlen)

* Lehrbücher zu digitaler Signalverarbeitung (DSP) für Grundlagen FIR/IIR/Konzept der Frequenzantwort
* Kalman-Filter Einführungen (z. B. "An Introduction to the Kalman Filter" von Welch & Bishop)
* Adaptive Filter / LMS / RLS Tutorials (z. B. Haykin: Adaptive Filter Theory)

---
