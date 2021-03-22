#include "iir_filter.h"

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

float iir_zeros[IIR_ZEROS_SIZE] = {
	-1.0000000000000000e+000  
};
// iir_poles

float iir_poles[IIR_POLES_SIZE] = {
	 9.0211475344416348e-001  
};
// Direct Form
// iir_numerator

float iir_b[IIR_B_SIZE] = {
	 4.8942621797323227e-002, 
	 4.8942621797323227e-002  
};
// iir_denominator

float iir_a[IIR_A_SIZE] = {
	 1.0000000000000000e+000, 
	-9.0211474895477295e-001  
};
// iir_biquads

float iir_biquads[IIR_BIQUADS_SIZE][6] = {
	{
		 1.0000000000000000e+000,     // B0
		 1.0000000000000000e+000,     // B1
		 0.0000000000000000e+000,     // B2
		 1.0000000000000000e+000,     // A0
		-9.0211474895477295e-001,     // A1
		 0.0000000000000000e+000      // A2
	}
};
// iir_biquads gain

float iir_biquads_g  =  4.8942623277918262e-002;


