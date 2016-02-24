#include "MyFirstPlugin.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include "Oscillator.h"
#include "EnvelopeStuff.h"



const int kNumPrograms = 5;

//enum used to keep track of params based on their index
enum EParams
{
  //these two were for the knobs
  //kFrequency = 0,
  kThreshold = 0,
  kAttack = 1,
  kDecay = 2,
  kSustain = 3,
  kRelease = 4,
  kWaveform1 = 5,
  kWaveform2,

  kFilterMode,
  kFilterCutoff,
  kFilterResonance,
  kFilterAttack,
  kFilterDecay,
  kFilterSustain,
  kFilterRelease,
  kFilterEnvelopeAmount,

  kLFOWaveform,
  kLFOFrequency,
  kLFOModAmount,
  kMix,
  kNoiseAmount,

  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,


};




///=====================================================================================================
///constructor
/*
plugin constructor
- initialise member variables
- add plugin parameters
- create gui and attach controls(such as knobs, sliders, etc)
- create factory presets
*/
// HUNTER, DERNAY, ETC this is the constructor for the Jeffrey with MIDI/oscillator only, KNOBS are in above constructor
MyFirstPlugin::MyFirstPlugin(IPlugInstanceInfo instanceInfo):IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
{
    TRACE;

	//============================================================================
	//  PARAMETERS
	//============================================================================
	///THRESHOLD (distortion knob)    name, defaultVal, minVal, maxVal, step, label
    GetParam(kThreshold)->InitDouble("Threshold", 0.01, 0.01, 100.0, 0.01, "%");
    GetParam(kThreshold)->SetShape(1.);

	///ADSR ENVELOPE
	//ATTACK (attack knob) limited to 2 seconds
    GetParam(kAttack)->InitDouble("Attack", 0.01, 0.01, 2.0, 0.01, "Sec");
    GetParam(kAttack)->SetShape(1.);
	//DECAY (decay knob) limited to one second
    GetParam(kDecay)->InitDouble("Decay", 0.5, 0.01, 1.0, 0.01, "Sec");
    GetParam(kDecay)->SetShape(1.);
	//SUSTAIN (sustain knob) maxes at .7 so there will actually be decay
    GetParam(kSustain)->InitDouble("Sustain", 0.3, 0.01, 0.7, 0.01, "Lvl");
    GetParam(kSustain)->SetShape(1.);
	//RELEASE (release knob) limited to 2 seconds
    GetParam(kRelease)->InitDouble("Release", 1, 0.01, 2.0, 0.01, "Sec");
    GetParam(kRelease)->SetShape(1.);

	
	///FILTER AND FILTER ENVELOPE PARAMS
	// FILTER CUTOFF
	GetParam(kFilterCutoff)->InitDouble("Cutoff", 0.99, 0.01, 0.99, 0.001);
	GetParam(kFilterCutoff)->SetShape(2);
	// FILTER RESONANCE
	GetParam(kFilterResonance)->InitDouble("Resonance", 0.01, 0.01, 1.0, 0.001);
	// FILTER ATTACK
	GetParam(kFilterAttack)->InitDouble("Filter Env Attack", 0.01, 0.01, 10.0, 0.001);
	GetParam(kFilterAttack)->SetShape(3);
	// FILTER DECAY
	GetParam(kFilterDecay)->InitDouble("Filter Env Decay", 0.5, 0.01, 15.0, 0.001);
	GetParam(kFilterDecay)->SetShape(3);
	// FILTER SUSTAIN
	GetParam(kFilterSustain)->InitDouble("Filter Env Sustain", 0.1, 0.001, 1.0, 0.001);
	GetParam(kFilterSustain)->SetShape(2);
	//FILTER RELEASE
	GetParam(kFilterRelease)->InitDouble("Filter Env Release", 1.0, 0.001, 15.0, 0.001);
	GetParam(kFilterRelease)->SetShape(3);
	//FILTER ENV AMOUNT
	GetParam(kFilterEnvelopeAmount)->InitDouble("Filter Env Amount", 0.0, -1.0, 1.0, 0.001);

	///LFO PARAMS
	//LFO FREQUENCY
	GetParam(kLFOFrequency)->InitDouble("LFO Frequency", 6.0, 0.01, 12.0, 0.01);
	//LFO MOD AMOUNT
	GetParam(kLFOModAmount)->InitDouble("LFO Mod Amount", 0.0, 0.0, 1.0, 0.001);
	//LFO WAVEFORM
	GetParam(kLFOWaveform)->InitEnum("LFO Waveform", OSCILLATOR_MODE_SINE, 4);



	///WAVEFORM 1
 	GetParam(kWaveform1)->InitEnum("Waveform1", OSCILLATOR_MODE_SINE, 4);
    GetParam(kWaveform1)->SetDisplayText(0, "Sine"); // Needed for VST3, thanks plunntic

	///WAVEFORM 2
 	GetParam(kWaveform2)->InitEnum("Waveform2", OSCILLATOR_MODE_SINE, 4);
    GetParam(kWaveform2)->SetDisplayText(0, "Sine");

	///FILTER MODE
	GetParam(kFilterMode)->InitEnum("Filter Mode", Filter::FILTER_MODE_LOWPASS, Filter::kNumFilterModes);

	/// OSCILLATOR MIX
	GetParam(kMix)->InitDouble("Oscillator Mix", 0.5, 0.0, 1.0, 0.001);
	GetParam(kMix)->SetShape(1.);

	///Noise Osc Amount
	GetParam(kNoiseAmount)->InitDouble("Oscillator Mix", 0.01, 0.01, 1.0, 0.001);


	//============================================================================
	//  GUI COMPONENTS
	//============================================================================

	///overall gui initialization
    IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
    pGraphics->AttachPanelBackground(&COLOR_TRANSPARENT);

	// Waveform switches 1 AND 2
    IBitmap waveformBitmap = pGraphics->LoadIBitmap(WAVEFORM_ID, WAVEFORM_FN, 4);
    pGraphics->AttachControl(new ISwitchControl(this, 50, 350, kWaveform1, &waveformBitmap));
	pGraphics->AttachControl(new ISwitchControl(this, 200, 350, kWaveform2, &waveformBitmap));

	// Filtermode Switch
	IBitmap filtermodeBitmap = pGraphics->LoadIBitmap(FILTERMODE_ID, FILTERMODE_FN, 3);
	pGraphics->AttachControl(new ISwitchControl(this, 50, 275, kFilterMode, &filtermodeBitmap));

	
	/// BITMAP FOR ALL CURRENT KNOBS
	IBitmap smallKnobBitmap = pGraphics->LoadIBitmap(KNOB_SMALL_ID, KNOB_SMALL_FN, 31);

	/// DISTORTION KNOB
	pGraphics->AttachControl(new IKnobMultiControl(this, 500, 300, kThreshold, &smallKnobBitmap));

	///ENVELOPE KNOBS
	//ATTACK KNOB
    pGraphics->AttachControl(new IKnobMultiControl(this, 50, 50, kAttack, &smallKnobBitmap));
	//DECAY KNOB
    pGraphics->AttachControl(new IKnobMultiControl(this, 250, 50, kDecay, &smallKnobBitmap));
	//SUSTAIN KNOB
    pGraphics->AttachControl(new IKnobMultiControl(this, 450, 50, kSustain, &smallKnobBitmap));
	//RELEASE KNOB
    pGraphics->AttachControl(new IKnobMultiControl(this, 650, 50, kRelease, &smallKnobBitmap));


	///FILTER AND FILTER ENVELOPE KNOBS
	//CUTOFF KNOB
	pGraphics->AttachControl(new IKnobMultiControl(this, 100, 275, kFilterCutoff, &smallKnobBitmap));
	//RESONANCE KNOB
	pGraphics->AttachControl(new IKnobMultiControl(this, 165, 275, kFilterResonance, &smallKnobBitmap));
	//FILTER ATTACK KNOB
	pGraphics->AttachControl(new IKnobMultiControl(this, 100, 215, kFilterAttack, &smallKnobBitmap));
	//FILTER DECAY KNOB
	pGraphics->AttachControl(new IKnobMultiControl(this, 165, 215, kFilterDecay, &smallKnobBitmap));
	// FILTER SUSTAIN KNOB
	pGraphics->AttachControl(new IKnobMultiControl(this, 230, 215, kFilterSustain, &smallKnobBitmap));
	// FILTER RELEASE KNOB	
	pGraphics->AttachControl(new IKnobMultiControl(this, 295, 215, kFilterRelease, &smallKnobBitmap));
	// FILTER AMOUNT KNOB	
	pGraphics->AttachControl(new IKnobMultiControl(this, 360, 215, kFilterEnvelopeAmount, &smallKnobBitmap));

	///LFO KNOBS
	// LFO FREQ KNOB	
	pGraphics->AttachControl(new IKnobMultiControl(this, 550, 350, kLFOFrequency, &smallKnobBitmap));
	// LFO MOD AMOUNT KNOB	
	pGraphics->AttachControl(new IKnobMultiControl(this, 600, 350, kLFOModAmount, &smallKnobBitmap));
	// LFO waveform select
	pGraphics->AttachControl(new ISwitchControl(this, 499, 350, kLFOWaveform, &waveformBitmap));

	///OSCILLATOR MIX KNOB
	pGraphics->AttachControl(new IKnobMultiControl(this, 125, 300, kMix, &smallKnobBitmap));

	///Noise Amount KNOB
	pGraphics->AttachControl(new IKnobMultiControl(this, 225, 300, kNoiseAmount, &smallKnobBitmap));

	///finalizing GUI
    AttachGraphics(pGraphics);
    CreatePresets();


	//attaching the signals in the MidiReceiver to the VoiceManager functions since they now handle notes
	mMidiRec.noteOn.Connect(&LilJeffreyManager, &VoiceManager::onNoteOn);
    mMidiRec.noteOff.Connect(&LilJeffreyManager, &VoiceManager::onNoteOff);

}


MyFirstPlugin::~MyFirstPlugin() {}




/*
- Called by the plugin host every time it
needs to process a block of samples
- Passes pointers to buffers of double
precision samples for each input and output
channel
- IPlug guarantees valid buffers, just full of
zeros if the host hasn’t connected them
*/
///processDoubleReplacing is the DSP part
void MyFirstPlugin::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
    // Mutex is already locked for us.

    double *leftOutput = outputs[0];
    double *rightOutput = outputs[1];

	double beforeDistortion;
	double afterDistortion;

	

    for (int i = 0; i < nFrames; ++i) {
		//makes sure we're generating the right pitch at the right moment
		//this is the function thaty keeps LastVelocity and LastFrequency correct
        mMidiRec.advance();

		//the velocity is how the note was pressed and equates to volume,
		//i think only midi keyboards will send this as other than 127(aka full volume)
        //int velocity = mMidiRec.getLastVelocity();

		//LilJeffrey.setFrequency(mMidiRec.getLastFrequency());


		///TESTING LFO HERE
		//double lfoFilterModulation = mLFO.nextSample() * lfoFilterModAmount;
        
        //mFilter.setCutoffMod((mFilterEnvGen.nextSample() * filterEnvelopeAmount) + lfoFilterModulation);
        //beforeDistortion = mFilter.process(LilJeffrey.nextSample() * mEnvGen.nextSample() * velocity / 127.0);
		beforeDistortion = LilJeffreyManager.nextSample();



		//needed for filter envelope
		//mFilter.setCutoffMod(mFilterEnvGen.nextSample() * filterEnvelopeAmount);

		///notice here that the envelope value is also used now :)
		//beforeDist is the sample before the clipping (Vel/127 equates to volume)
	    //beforeDistortion = mFilter.process(LilJeffrey.nextSample() * mEnvGen.nextSample() *  (velocity / 127.0));







		if(beforeDistortion >= 0) {
		  // Make sure positive values can't go above the Frequency:
		   if (beforeDistortion > mThreshold)
			  afterDistortion = mThreshold;
		   else
			  afterDistortion = beforeDistortion;
        } 
		else {
		  // Make sure negative values can't go below the Frequency:
		   if (beforeDistortion < -mThreshold)
			  afterDistortion = -mThreshold;
		   else
			  afterDistortion = beforeDistortion;
        }
		
		//compensating for the loss of volume due to the clipping
	    afterDistortion /= mThreshold;

		//setting the output buffers equal to the final value to be outputted aka played                                                         
        leftOutput[i] = rightOutput[i] = afterDistortion;
    }

    mMidiRec.Flush(nFrames);
}


///Called when the processing is started, or restarted, e.g. on change of samplerate
/*
- use GetSampleRate() to check the new
sample rate and update your DSP
accordingly
*/

void MyFirstPlugin::Reset()
{
  TRACE;
  IMutexLock lock(this);
  //sets the oscillator's SR to the Iplug's value
  LilJeffreyManager.setSampleRate(GetSampleRate());
}





///Called for each parameter change
/*
- whenever a parameter is automated
- when you edit a parameter via the GUI
- when you recall a preset
*/
void MyFirstPlugin::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);



  //  NEEDED IN ITERATION WITH 2 KNOBS

  //needs to modify all params
  switch (paramIdx)
  {
	 
    case kThreshold:
		//sets the member value equal to the param value from the knob
         mThreshold = 1 - (GetParam(kThreshold)->Value()/ 100.);
      break;

	case kAttack:
		//sets the member value equal to the param value from the knob
		LilJeffreyManager.setVolEnvAll(STAGE_ATTACK, (GetParam(kAttack)->Value()) );
      break;

	case kDecay:
		//sets the member value equal to the param value from the knob
		LilJeffreyManager.setVolEnvAll(STAGE_DECAY, (GetParam(kDecay)->Value())  );
      break;

	case kSustain:
		//sets the member value equal to the param value from the knob
		LilJeffreyManager.setVolEnvAll(STAGE_SUSTAIN, (GetParam(kSustain)->Value())  );
      break;

	case kRelease:
		//sets the member value equal to the param value from the knob
		LilJeffreyManager.setVolEnvAll(STAGE_RELEASE, (GetParam(kRelease)->Value())  );
      break;

	case kWaveform1:
		LilJeffreyManager.setOsc1WaveformAll(static_cast<OscillatorMode>(GetParam(kWaveform1)->Int()));
      break;

	case kWaveform2:
		LilJeffreyManager.setOsc2WaveformAll(static_cast<OscillatorMode>(GetParam(kWaveform2)->Int()));
      break;

	case kFilterCutoff:
		LilJeffreyManager.setFilterCutoffAll(GetParam(paramIdx)->Value());
      break;

	case kFilterResonance:
		LilJeffreyManager.setFilterResAll(GetParam(paramIdx)->Value());
      break;

	case kFilterMode:
		LilJeffreyManager.setFilterModeAll(static_cast<Filter::FilterMode>(GetParam(paramIdx)->Int()));
      break;

	case kFilterAttack:
		LilJeffreyManager.setFilterEnvAll(STAGE_ATTACK, (GetParam(kFilterAttack)->Value())  );
      break;

	case kFilterDecay:
		LilJeffreyManager.setFilterEnvAll(STAGE_DECAY, (GetParam(kFilterDecay)->Value())  );
	  break;

	case kFilterSustain:
		LilJeffreyManager.setFilterEnvAll(STAGE_SUSTAIN, (GetParam(kFilterSustain)->Value())  );
	  break;

	case kFilterRelease:
		LilJeffreyManager.setFilterEnvAll(STAGE_RELEASE, (GetParam(kFilterRelease)->Value())  );

	case kFilterEnvelopeAmount:
		LilJeffreyManager.setFilterEnvAmtAll(GetParam(paramIdx)->Value());
	  break;

	case kLFOFrequency:
		LilJeffreyManager.setLFOFrequency(GetParam(paramIdx)->Value());
	  break;

    case kLFOModAmount:
		LilJeffreyManager.setLFOAmountAll(GetParam(paramIdx)->Value());
	  break;

    case kLFOWaveform:
		LilJeffreyManager.setLFOMode(static_cast<OscillatorMode>(GetParam(kLFOWaveform)->Int()));
      break;

    case kMix:
		LilJeffreyManager.setOscMixAll(GetParam(paramIdx)->Value());
      break;

	case kNoiseAmount:
		LilJeffreyManager.setNoiseAmountAll(GetParam(paramIdx)->Value());
      break;

    default:
      break;
  }
  
}



void MyFirstPlugin::CreatePresets() {
    MakeDefaultPreset((char *) "-", kNumPrograms);
}



///this is an overwrite of the IPlug function, gets called every time a MIDI message is received by VST
//just calls onMessageReceived with the message 
void MyFirstPlugin::ProcessMidiMsg(IMidiMsg* pMsg) {
    mMidiRec.onMessageReceived(pMsg);
}