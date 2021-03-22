#ifndef _IIR_FILTER_H_
#define _IIR_FILTER_H_

// Specification
// Fs = 120 Hz
// Order = 1
// Gain = 0 dB
// Filter Type = Chebyshev
// Response = Lowpass
// Data Type =IEEE 4-byte
// Cutoff frequency = 120 Hz
// Passband ripple = 1 dB

// iir_zeros

#define IIR_ZEROS_SIZE 1
extern float iir_zeros[IIR_ZEROS_SIZE];
// iir_poles

#define IIR_POLES_SIZE 1
extern float iir_poles[IIR_POLES_SIZE];
// Direct Form
// iir_numerator

#define IIR_B_SIZE 2
extern float iir_b[IIR_B_SIZE];
// iir_denominator

#define IIR_A_SIZE 2
extern float iir_a[IIR_A_SIZE];
// iir_biquads

#define IIR_BIQUADS_SIZE 1
extern float iir_biquads[IIR_BIQUADS_SIZE][6];
// iir_biquads gain

extern float iir_biquads_g ;


#endif

