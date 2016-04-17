#pragma once
#include <cmath>
#include <math.h>
#include <algorithm>
using namespace std;



// Had to do a weird workaround because fmin and fmax were undefined
/*
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

//#undef max
//#undef min*/




class Filter {
public:

    enum FilterMode {
        FILTER_MODE_LOWPASS = 0,
        FILTER_MODE_HIGHPASS,
        FILTER_MODE_BANDPASS,
        kNumFilterModes
    };

    Filter();
    double process(double inputValue);
    inline void setCutoff(double newCutoff) { cutoff = newCutoff; calculateFeedbackAmount(); };
    inline void setResonance(double newResonance) { resonance = newResonance; calculateFeedbackAmount(); };
    inline void setFilterMode(FilterMode newMode) { mode = newMode; }
	inline void setCutoffMod(double newCutoffMod) { cutoffMod = newCutoffMod; calculateFeedbackAmount(); }
	void reset();

private:
    double cutoff;
    double resonance;
	double cutoffMod;


	inline double getCalculatedCutoff() const {
		//max and min defined up top
		return max(min(cutoff + cutoffMod, 0.99), 0.01);	// was gonna use fmax and fmin but it was undefined
	};

	//max of roughly 2
	inline void calculateFeedbackAmount() {
		feedbackAmount = resonance + resonance/(1.0 - getCalculatedCutoff());
	}

	//indicates wwhat mode (hi, low, band) we're in
    FilterMode mode;
    double feedbackAmount;
    double buf0;
    double buf1;
	double buf2;
	double buf3;
};