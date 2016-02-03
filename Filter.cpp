#include "Filter.h"

double Filter::process(double inputValue) {
	// note: use only buf1 for -12db filter; buf3 for -24db filter
	// will possibly modify this later to add an option for either -12 or -24
	if (inputValue == 0.0) return inputValue;
	double calculatedCutoff = getCalculatedCutoff();
	buf0 += calculatedCutoff * (inputValue - buf0 + feedbackAmount * (buf0 - buf1));
	buf1 += calculatedCutoff * (buf0 - buf1);
	buf2 += calculatedCutoff * (buf1 - buf2);
	buf3 += calculatedCutoff * (buf2 - buf3);

    switch (mode) {
        case FILTER_MODE_LOWPASS:
            return buf3;
        case FILTER_MODE_HIGHPASS:
            return inputValue - buf3;
        case FILTER_MODE_BANDPASS:
            return buf0 - buf3;
        default:
            return 0.0;
    }
}