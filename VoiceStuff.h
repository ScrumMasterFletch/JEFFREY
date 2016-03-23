#include "Oscillator.h"
#include "EnvelopeStuff.h"
#include "Filter.h"

#ifndef __VOICESTUFF__
#define __VOICESTUFF__

class Voice{

private:
    Oscillator oscillatorOne;
    Oscillator oscillatorTwo;
    EnvGen volEnvelope;
    EnvGen filterEnvelope;
    Filter filter;
	//for MIDI
	int noteNum;
    int velocity;
	double filterEnvAmount;
    double oscillatorMix;
    double filterLFOAmount;
    double oscillatorOnePitchAmount;
    double oscillatorTwoPitchAmount;
    double lfoValue;
	double noiseAmount;
	

public:
	friend class VoiceManager;
	Voice();
	void reset();
	inline void setFilterEnvelopeAmount(double amount) { filterEnvAmount = amount; }
    inline void setFilterLFOAmount(double amount) { filterLFOAmount = amount; }
    inline void setOscillatorOnePitchAmount(double amount) { oscillatorOnePitchAmount = amount; }
    inline void setOscillatorTwoPitchAmount(double amount) { oscillatorTwoPitchAmount = amount; }
    inline void setOscillatorMix(double mix) { oscillatorMix = mix; }
    inline void setLFOValue(double value) { lfoValue = value; }
	inline void setNoiseAmount(double value) { noiseAmount = value; }

	//calculates the frequency based on the note number and passes it to both oscillators
    inline void setNoteNumber(int noteNumber) {
        noteNum = noteNumber;
        double frequency = 440.0 * pow(2.0, (noteNum - 69.0) / 12.0);
        oscillatorOne.setFrequency(frequency);
        oscillatorTwo.setFrequency(frequency);
    }
	bool isActive;
	double nextSample();
    void setFree();

	

};


#endif