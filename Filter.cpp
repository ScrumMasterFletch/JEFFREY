#include "Filter.h"

Filter::Filter(){
    cutoff = 0.99;
    resonance = 0.01;
	cutoffMod = 0.0;
    mode = FILTER_MODE_LOWPASS;
    buf0 = 0.0;
    buf1 = 0.0;
	buf2 = 0.0;
	buf3 = 0.0;
    
    calculateFeedbackAmount();
}


double Filter::process(double inputValue) {
	// note: use only buf1 for -12db filter; buf3 for -24db filter
	// will possibly modify this later to add an option for either -12 or -24
	if (inputValue == 0.0) return inputValue;
	double calculatedCutoff = getCalculatedCutoff();
	buf0 += calculatedCutoff * (inputValue - buf0 + feedbackAmount * (buf0 - buf1));
	buf1 += calculatedCutoff * (buf0 - buf1);
	buf2 += calculatedCutoff * (buf1 - buf2);
	//buf3 is the low frequencies
	buf3 += calculatedCutoff * (buf2 - buf3);

    switch (mode) {
		//low pass filters only allow frequencies beLOW the cutoff 
        case FILTER_MODE_LOWPASS:
            return buf3;
		//high pass filters allow frequencies below the cutoff (by subtracting the low values)
        case FILTER_MODE_HIGHPASS:
            return inputValue - buf3;

        case FILTER_MODE_BANDPASS:
            return buf0 - buf3;
        default:
            return 0.0;
    }
}

void Filter::reset() {
    buf0 = buf1 = buf2 = buf3 = 0.0;
}