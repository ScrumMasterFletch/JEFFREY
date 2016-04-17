#include "VoiceStuff.h"

Voice::Voice(){

    noteNum = -1;
    velocity = 0;
    filterEnvAmount = 0.0;
    filterLFOAmount = 0.0;
    oscillatorOnePitchAmount = 0.0;
    oscillatorTwoPitchAmount = 0.0;
    oscillatorMix = 0.5;
    lfoValue = 0.0;
    isActive = false;
	noiseAmount = 0.0;
	glideTime = 0.0;

    // Set myself free everytime my volume envelope has fully faded out of RELEASE stage
	volEnvelope.finishedEnvelopeCycle.Connect(this, &Voice::setFree);
    
}


///nextSample() returns the entire sample from the Voice, which consists of
/// two summed oscillators (after volume envelope, filter envelope, and LFO have been applied)
double Voice::nextSample() {
    if (!isActive) return 0.0;

	//oscillatorOne.setPitchMod(lfoValue * oscillatorOnePitchAmount);
    //oscillatorTwo.setPitchMod(lfoValue * oscillatorTwoPitchAmount);

	//grabbing a sample from both oscillators
    double oscOneSample = oscillatorOne.nextSample();
    double oscTwoSample = oscillatorTwo.nextSample();

	//summing the oscillators together after theyve been multiplied by the correct mix amount
    double oscillatorSum = (((1 - oscillatorMix) * oscOneSample) + (oscillatorMix * oscTwoSample)) + noiseAmount;

	//grabbing the samples from both envelopes
    double volumeEnvelopeValue = volEnvelope.nextSample();
    double filterEnvelopeValue = filterEnvelope.nextSample();

	//preparing the envelopes and LFO
    filter.setCutoffMod(filterEnvelopeValue * filterEnvAmount + lfoValue * filterLFOAmount);
	//filter.setCutoffMod(lfoValue * filterLFOAmount);

	oscillatorOne.setPitchMod(lfoValue * oscillatorOnePitchAmount);
    oscillatorTwo.setPitchMod(lfoValue * oscillatorTwoPitchAmount);
	//oscillatorOne.setPitchMod(lfoValue);
    //oscillatorTwo.setPitchMod(lfoValue);

	///piecing it all together
    return filter.process(oscillatorSum * volumeEnvelopeValue * velocity / 127.0);
}

void Voice::setFree() {
    isActive = false;
}

void Voice::reset() {
    noteNum = -1;
    velocity = 0;
    oscillatorOne.reset();
    oscillatorTwo.reset();
    volEnvelope.reset();
    filterEnvelope.reset();
    filter.reset();
}