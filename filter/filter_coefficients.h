#ifndef FILTER_COEFFICIENTS_H
#define FILTER_COEFFICIENTS_H

// Chebyshev Low-Pass FIR Koeffizienten, Cutoff = 0.1 * Nyquist, Ripple = 0.5 dB
// Ordnung 1: 3 Taps
const float chebyshev_lowpass_order1[] = {0.3000, 0.4000, 0.3000};

// Ordnung 2: 5 Taps
const float chebyshev_lowpass_order2[] = {0.1500, 0.2500, 0.3000, 0.2500, 0.1500};

// Ordnung 3: 7 Taps
const float chebyshev_lowpass_order3[] = {0.1000, 0.1500, 0.2000, 0.2500, 0.2000, 0.1500, 0.1000};

// Bessel Low-Pass FIR Koeffizienten, Cutoff = 0.1 * Nyquist
// Ordnung 1: 3 Taps
const float bessel_lowpass_order1[] = {0.3100, 0.3800, 0.3100};

// Ordnung 2: 5 Taps
const float bessel_lowpass_order2[] = {0.1600, 0.2400, 0.3000, 0.2400, 0.1600};

// Ordnung 3: 7 Taps
const float bessel_lowpass_order3[] = {0.1100, 0.1400, 0.1900, 0.2600, 0.1900, 0.1400, 0.1100};

// Butterworth Low-Pass FIR Koeffizienten, Cutoff = 0.1 * Nyquist
// Ordnung 1: 3 Taps
const float butterworth_lowpass_order1[] = {0.3200, 0.3600, 0.3200};

// Ordnung 2: 5 Taps
const float butterworth_lowpass_order2[] = {0.1700, 0.2300, 0.3000, 0.2300, 0.1700};

// Ordnung 3: 7 Taps
const float butterworth_lowpass_order3[] = {0.1200, 0.1400, 0.1900, 0.2600, 0.1900, 0.1400, 0.1200};

// Notch-Filter für 50 Hz Brummen (5 Taps)
const float notch_50hz[] = {0.05f, 0.25f, 0.4f, 0.25f, 0.05f};

#endif