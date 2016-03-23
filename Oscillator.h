


#ifndef __OSCILLATOR__
#define __OSCILLATOR__

#include <math.h>

enum OscillatorMode {
    OSCILLATOR_MODE_SINE,
    OSCILLATOR_MODE_SAW,
    OSCILLATOR_MODE_SQUARE,
    OSCILLATOR_MODE_TRIANGLE,
	OSCILLATOR_MODE_NOISE,
	kNumOscillatorModes
};

class Oscillator {
private:
    OscillatorMode mOscillatorMode;
    double mPI;
    double mFrequency;
    double mPhase;
    static double mSampleRate;
    double mPhaseIncrement;
    void updateIncrement();
	double twoPI;
    bool isMuted;
public:
	void reset() { mPhase = 0.0; }
    void setMode(OscillatorMode mode);
    void setFrequency(double frequency);
    void setSampleRate(double sampleRate);
    //void generate(double* buffer, int nFrames);
	inline void setMuted(bool muted) { isMuted = muted; }
	double nextSample();
    Oscillator();
        
};

#endif