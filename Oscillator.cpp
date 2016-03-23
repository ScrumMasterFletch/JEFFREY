#include "Oscillator.h"
#include <cstdlib>

//all oscillators will have the same sample rate guaranteed
double Oscillator::mSampleRate = 44100.0;

///	constructor
Oscillator::Oscillator(){
mOscillatorMode = OSCILLATOR_MODE_SINE;
        mPI = (2*acos(0.0));
		twoPI = 2 * mPI; 
        isMuted = true;
        mFrequency = 440.0;
        mPhase = 0.0;
        //mSampleRate = (44100.0);
		updateIncrement();
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

void Oscillator::updateIncrement() {
    mPhaseIncrement = mFrequency * 2 * mPI / mSampleRate;
}




double Oscillator::nextSample() {
    double value = 0.0;
    

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
            //value = (rand() % 21  + (-10)) / 10;
			value = (rand() % 11  + (-5)) / 5;
            break;
    }
    mPhase += mPhaseIncrement;
    while (mPhase >= twoPI) {
        mPhase -= twoPI;
    }
    return value;
}