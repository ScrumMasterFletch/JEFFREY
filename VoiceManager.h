#include "VoiceStuff.h"

class VoiceManager{

private:
	static const int maxVoices = 15;
    Voice voiceArray[maxVoices];
    Oscillator LFO; //can be global to all voices
	Oscillator Noise;
    Voice* findFreeVoice();
	double noiseAmount;
	Filter noiseFilter;
	double VMGlideTime;
	
	

public:
	//glide note trackers
	int currGlideNote;
	int prevGlideNote;
	void setGlideTime(double amount);


	VoiceManager();
	//used with MIDI
	void onNoteOn(int noteNum, int vel);
    void onNoteOff(int noteNum, int vel);
	void setSampleRate(double sr);
    double nextSample();
	inline void setLFOMode(OscillatorMode mode) { LFO.setMode(mode); };
    inline void setLFOFrequency(double frequency) { LFO.setFrequency(frequency); };

	///polyphony setters
	void setFilterCutoffAll(double newCutoff);
	void setFilterResAll(double res);
	void setFilterModeAll(Filter::FilterMode newMode);

	//used for att, dec, sust, rel
	void setVolEnvAll(int stage, double value);
	void setFilterEnvAll(int stage, double value);
	void setLFOAmountAll(double amount);
	void setLFOFrequencyAll(double freq);
	void setLFOWaveformAll(OscillatorMode mode);
	void setOsc1WaveformAll(OscillatorMode mode);
	void setOsc2WaveformAll(OscillatorMode mode);
	void setFilterEnvAmtAll(double amount);
	void setOscMixAll(double mix);

	void setNoiseAmountAll(double amount);
	void setNoiseFilterCutoff(double amount);

	void setOscillatorPitchModAll(int oscillatorNumber, double amount); 


};