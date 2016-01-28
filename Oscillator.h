


#ifndef __OSCILLATOR__
#define __OSCILLATOR__

#include <math.h>

enum OscillatorMode {
    OSCILLATOR_MODE_SINE,
    OSCILLATOR_MODE_SAW,
    OSCILLATOR_MODE_SQUARE,
    OSCILLATOR_MODE_TRIANGLE
};

class Oscillator {
private:
    OscillatorMode mOscillatorMode;
    double mPI;
    double mFrequency;
    double mPhase;
    double mSampleRate;
    double mPhaseIncrement;
    void updateIncrement();
	double twoPI;
    bool isMuted;
public:
    void setMode(OscillatorMode mode);
    void setFrequency(double frequency);
    void setSampleRate(double sampleRate);
    //void generate(double* buffer, int nFrames);
	inline void setMuted(bool muted) { isMuted = muted; }
	double nextSample();
    Oscillator();
        
};

#endif