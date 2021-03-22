/**************************************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: Low Pass
Filter model: Chebyshev
Filter order: 2
Sampling Frequency: 500 Hz
Cut Frequency: 1.000000 Hz
Pass band Ripple: 1.000000 dB
Coefficents Quantization: float

Z domain Zeros
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000

Z domain Poles
z = 0.993064 + j -0.011171
z = 0.993064 + j 0.011171
***************************************************************/
#define NCoef 2
float iir(float NewSample) {
    float ACoef[NCoef+1] = {
        0.00004394049036616966,
        0.00008788098073233933,
        0.00004394049036616966
    };

    float BCoef[NCoef+1] = {
        1.00000000000000000000,
        -1.98612746371031550000,
        0.98630036962500822000
    };

    static float y[NCoef+1]; //output samples
    static float x[NCoef+1]; //input samples
    int n;

    //shift the old samples
    for(n=NCoef; n>0; n--) {
       x[n] = x[n-1];
       y[n] = y[n-1];
    }

    //Calculate the new output
    x[0] = NewSample;
    y[0] = ACoef[0] * x[0];
    for(n=1; n<=NCoef; n++)
        y[0] += ACoef[n] * x[n] - BCoef[n] * y[n];
    
    return y[0];
}
