#include "Oscillator.h"
#include <cstdlib>


//to convert to wavetable, need to initialize the wavetables in MyFirstPlugin constructor (take code from OG Jeff)
// - convert each sample returner to return the wave table sample
// - possibly convert frequency calculating stuff from MIDI
// - call that shit at each time frequency changes?

//all oscillators will have the same sample rate guaranteed
double Oscillator::mSampleRate = 44100.0;

///	constructor
Oscillator::Oscillator(){

        mOscillatorMode = OSCILLATOR_MODE_SINE;

        glideFloat = 0;
        frequencyGoal = 0;
		
		mPI = (2*acos(0.0));
		twoPI = 2 * mPI; 
        isMuted = true;
        mFrequency = 440.0;
        mPhase = 0.0;
        //mSampleRate = (44100.0);
		updateIncrement();
		pitchMod = 0.0;
}



void Oscillator::setGlideFloat(double gf){

	glideFloat = gf;

}



void Oscillator::setFrequencyGoal(double freq){

	frequencyGoal = freq;

}

//SETTERS
void Oscillator::setMode(OscillatorMode mode) {
    mOscillatorMode = mode;
}

void Oscillator::setFrequency(double frequency) {
    mFrequency = frequency;
    updateIncrement();
}

void Oscillator::setSampleRate(double sampleRate) {
    mSampleRate = sampleRate;
    updateIncrement();
}

//new update increment to be used with frequency modulation
void Oscillator::updateIncrement() {

    double pitchModAsFrequency = pow(2.0, fabs(pitchMod) * 14.0) - 1;

    if (pitchMod < 0) {
        pitchModAsFrequency = -pitchModAsFrequency;
    }

	//double calFreq = min(max(mFrequency, 0), mSampleRate/2.0);
	double calFreq = min(max(mFrequency + pitchModAsFrequency, 0), mSampleRate/2.0);

	//double calFreq = mFrequency + pitchModAsFrequency;
	mPhaseIncrement = calFreq * 2 * mPI / mSampleRate;

}


void Oscillator::setPitchMod(double amount) {
    pitchMod = amount;
    updateIncrement();
}




double Oscillator::nextSample() {
    double value = 0.0;
    double white = 0.0;
	double b0 = 0.0;
	double b1 = 0.0;
	double b2 = 0.0;
	double b3 = 0.0;
	double b4 = 0.0;
	double b5 = 0.0;
	double b6 = 0.0;
	

	

    switch (mOscillatorMode) {
        case OSCILLATOR_MODE_SINE:
            value = sin(mPhase);
            break;
        case OSCILLATOR_MODE_SAW:
            value = 1.0 - (2.0 * mPhase / twoPI);
            break;
        case OSCILLATOR_MODE_SQUARE:
            if (mPhase <= mPI) {
                value = 1.0;
            } else {
                value = -1.0;
            }
            break;
        case OSCILLATOR_MODE_TRIANGLE:
            value = -1.0 + (2.0 * mPhase / twoPI);
            value = 2.0 * (fabs(value) - 0.5);
            break;
        case OSCILLATOR_MODE_NOISE: ///need to add an instance of this to the voice manager, and setter/params/knob
			//generating the white noise samples
            white = (rand() % 21  + (-10)) / 10;
			
			//filtering the white noise in to pink noise using Paul Kellet's refined method in Allan's analysis
			//found at http://www.firstpr.com.au/dsp/pink-noise/#Pseudo
			b0 = 0.99886 * b0 + white * 0.0555179; 
            b1 = 0.99332 * b1 + white * 0.0750759; 
            b2 = 0.96900 * b2 + white * 0.1538520; 
            b3 = 0.86650 * b3 + white * 0.3104856; 
            b4 = 0.55000 * b4 + white * 0.5329522; 
            b5 = -0.7616 * b5 - white * 0.0168980; 
			b6 = white * 0.115926; 
            value = b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362;
            break;
    }

	//calculation for glide changes to 
	if (glideFloat != 0){
		mFrequency = mFrequency + glideFloat;
		if (glideFloat > 0){
			if(mFrequency > frequencyGoal)
				mFrequency = frequencyGoal;
		}
		else //
		{
			if(mFrequency < frequencyGoal)
				mFrequency = frequencyGoal;
		}
		mPhaseIncrement = mFrequency * 2 * mPI / mSampleRate;
	}

    mPhase += mPhaseIncrement;
    while (mPhase >= twoPI) {
        mPhase -= twoPI;
    }

    return value;
}