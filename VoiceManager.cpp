#include "VoiceStuff.h"
#include "VoiceManager.h"

VoiceManager::VoiceManager(){
	VMGlideTime = 0.0;

	Noise.setMode(OSCILLATOR_MODE_NOISE);

	noiseFilter.setFilterMode(Filter::FILTER_MODE_LOWPASS);

}


Voice* VoiceManager::findFreeVoice() {

    Voice* free = NULL;

    for (int i = 0; i < maxVoices; i++) {
		//checking if the current voice at the index is inactive
        if (voiceArray[i].isActive == false) {
			//free is now a reference to the inactive Voice
            free = &(voiceArray[i]);
            break;
        }
    }
    return free;
}

//this is called to handle grabbing a new voice and initializing it when a note is pressed
void VoiceManager::onNoteOn(int noteNumber, int velocity) {
    Voice* voice = findFreeVoice();
	//if voice actually exists, and findFreeVoice didnt just return NULL
    if (voice) 
	{
       voice->reset();

	   //this block is for glide
	   prevGlideNote = currGlideNote;
	   currGlideNote = noteNumber;
       voice->setNoteNumber(noteNumber, prevGlideNote);

	   //voice->setGlideTime(VMGlideTime);

       voice->velocity = velocity;
       voice->isActive = true;
       voice->volEnvelope.enterStage(STAGE_ATTACK);
       voice->filterEnvelope.enterStage(STAGE_ATTACK); 
    }
	//else findFreeVoice DID return NULL, and nothing should happen
	else return;
}

//called to end the notes associated with a certain note number when its off
void VoiceManager::onNoteOff(int noteNumber, int velocity) {

    //loops through the voice array looking for voices of the same note
    for (int i = 0; i < maxVoices; i++) {
        Voice& voice = voiceArray[i];
		//if the voice is active
        if(voice.isActive) 
		{	//check if the note number is the one we're looking for :)
		   if(voice.noteNum == noteNumber) 
		   {  //if so, see both of the voice's envelopes to release stage
              voice.volEnvelope.enterStage(STAGE_RELEASE);
              voice.filterEnvelope.enterStage(STAGE_RELEASE);

           }
		}

    }//for-loop
}

//summing all voices together for final output
double VoiceManager::nextSample() {
    double output = 0.0;
    double lfoValue = LFO.nextSample();
	double noiseValue = noiseFilter.process(Noise.nextSample()*noiseAmount);
    for (int i = 0; i < maxVoices; i++) {
        Voice& voice = voiceArray[i];
        voice.setLFOValue(lfoValue);
		voice.setNoiseAmount(noiseValue);
        output += voice.nextSample();
    }
	
    return output;
}

void VoiceManager::setSampleRate(double sr) {
	//setting the EnvGen
    EnvGen::setSampleRate(sr);

	//seting both oscillators of all voices
    for (int i = 0; i < maxVoices; i++) {
        Voice& voice = voiceArray[i];
        voice.oscillatorOne.setSampleRate(sr);
        voice.oscillatorTwo.setSampleRate(sr);
    }

	//setting the LFO
    LFO.setSampleRate(sr);
}




///setters for onParamChange
//Oscillator 1 waveform
void VoiceManager::setOsc1WaveformAll(OscillatorMode mode) {
    for (int i = 0; i < maxVoices; i++) {
		voiceArray[i].oscillatorOne.setMode(mode);
    }
}

//Oscillator 2 waveform
void VoiceManager::setOsc2WaveformAll(OscillatorMode mode) {
    for (int i = 0; i < maxVoices; i++) {
		voiceArray[i].oscillatorTwo.setMode(mode);
    }
}

//VOLUME ENVELOPE PARAMS
void VoiceManager::setVolEnvAll(int stage, double value) {
    for (int i = 0; i < maxVoices; i++) {
		voiceArray[i].volEnvelope.setStageValues(stage, value);
    }
}


//FILTER ENVELOPE PARAMS
void VoiceManager::setFilterEnvAll(int stage, double value) {
    for (int i = 0; i < maxVoices; i++) {
		voiceArray[i].filterEnvelope.setStageValues(stage, value);
    }
}

///env amount here

///LFO SETTERS==================================================
//LFO AMOUNT
void VoiceManager::setLFOAmountAll(double amount) {
    for (int i = 0; i < maxVoices; i++) {
		voiceArray[i].setFilterLFOAmount(amount);
    }
}

//LFO Frequency
void VoiceManager::setLFOFrequencyAll(double freq) {
    LFO.setFrequency(freq);
}

//LFO waveform  setMode(OscillatorMode mode)
void VoiceManager::setLFOWaveformAll(OscillatorMode mode) {
    LFO.setMode(mode);
}

///FILTER SETTERS================================================
void VoiceManager::setFilterCutoffAll(double newCutoff) {
    for (int i = 0; i < maxVoices; i++) {
        voiceArray[i].filter.setCutoff(newCutoff);
    }
}

void VoiceManager::setFilterResAll(double res) {
    for (int i = 0; i < maxVoices; i++) {
        voiceArray[i].filter.setResonance(res);
    }
}

void VoiceManager::setFilterModeAll(Filter::FilterMode newMode) {
    for (int i = 0; i < maxVoices; i++) {
        voiceArray[i].filter.setFilterMode(newMode);
    }
}

void VoiceManager::setFilterEnvAmtAll(double amount) {
    for (int i = 0; i < maxVoices; i++) {
        voiceArray[i].setFilterEnvelopeAmount(amount);
    }
}

//OSC MIX SETTER
void VoiceManager::setOscMixAll(double mix) {
    for (int i = 0; i < maxVoices; i++) {
		voiceArray[i].setOscillatorMix(mix);
    }
}

void VoiceManager::setNoiseAmountAll(double amount){
	noiseAmount = amount;
}

void VoiceManager::setNoiseFilterCutoff(double amount){
	noiseFilter.setCutoff(amount);
}

void VoiceManager::setOscillatorPitchModAll(int oscillatorNumber, double amount) {

	for (int i = 0; i < maxVoices; i++) {
		switch (oscillatorNumber) {
          case 1:
            voiceArray[i].setOscillatorOnePitchAmount(amount);
            break;
          case 2:
            voiceArray[i].setOscillatorTwoPitchAmount(amount);
            break;
        }//switch statement

    }//for loop
    
}


void VoiceManager::setGlideTime(double amount) {
    for (int i = 0; i < maxVoices; i++) {
		voiceArray[i].setGlideTime(amount);
    }
}