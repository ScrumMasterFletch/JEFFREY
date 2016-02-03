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
  kWaveform = 5,

  kFilterMode,
  kFilterCutoff,
  kFilterResonance,
  kFilterAttack,
  kFilterDecay,
  kFilterSustain,
  kFilterRelease,
  kFilterEnvelopeAmount,

  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  //kFrequencyX = 334,
  //kFrequencyY = 166,
  //kKnobFrames = 128
};

//distortion knob stats
enum ELayout2
{
  kThresholdX = 500,
  kThresholdY = 300,
  kKnobFrames2 = 128
};

//Attack knob stats
enum ELayout3
{
  kAttackX = 50,
  kAttackY = 50,
  kKnobFrames3 = 128
};

//Decay knob stats
enum ELayout4
{
  kDecayX = 250,
  kDecayY = 50,
  kKnobFrames4 = 128
};

//Sustain knob stats
enum ELayout5
{
  kSustainX = 450,
  kSustainY = 50,
  kKnobFrames5 = 128
};

//Release knob stats
enum ELayout6
{
  kReleaseX = 650,
  kReleaseY = 50,
  kKnobFrames6 = 128
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
MyFirstPlugin::MyFirstPlugin(IPlugInstanceInfo instanceInfo):IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo) {
    TRACE;

	//============================================================================
	//  PARAMETERS
	//============================================================================
	///THRESHOLD (distortion knob)    name, defaultVal, minVal, maxVal, step, label
    GetParam(kThreshold)->InitDouble("Threshold", 0.01, 0.01, 100.0, 0.01, "%");
    GetParam(kThreshold)->SetShape(1.);

	///ATTACK (attack knob) limited to 2 seconds
    GetParam(kAttack)->InitDouble("Attack", 0.01, 0.01, 2.0, 0.01, "Sec");
    GetParam(kAttack)->SetShape(1.);

	///DECAY (decay knob) limited to one second
    GetParam(kDecay)->InitDouble("Decay", 0.5, 0.01, 1.0, 0.01, "Sec");
    GetParam(kDecay)->SetShape(1.);

	///SUSTAIN (sustain knob) maxes at .7 so there will actually be decay
    GetParam(kSustain)->InitDouble("Sustain", 0.3, 0.01, 0.7, 0.01, "Lvl");
    GetParam(kSustain)->SetShape(1.);

	///RELEASE (release knob) limited to 2 seconds
    GetParam(kRelease)->InitDouble("Release", 1, 0.01, 2.0, 0.01, "Sec");
    GetParam(kRelease)->SetShape(1.);

	///Filter Envelope Knobs
		///ATTACK (attack knob) limited to 2 seconds
		GetParam(kFilterAttack)->InitDouble("Attack", 0.01, 0.01, 2.0, 0.01, "Sec");
		GetParam(kFilterAttack)->SetShape(1.);

		///DECAY (decay knob) limited to one second
		GetParam(kFilterDecay)->InitDouble("Decay", 0.5, 0.01, 1.0, 0.01, "Sec");
		GetParam(kFilterDecay)->SetShape(1.);

		///SUSTAIN (sustain knob) maxes at .7 so there will actually be decay
		GetParam(kFilterSustain)->InitDouble("Sustain", 0.3, 0.01, 0.7, 0.01, "Lvl");
		GetParam(kFilterSustain)->SetShape(1.);

		///RELEASE (release knob) limited to 2 seconds
		GetParam(kFilterRelease)->InitDouble("Release", 1, 0.01, 2.0, 0.01, "Sec");
		GetParam(kFilterRelease)->SetShape(1.);

	GetParam(kWaveform)->InitEnum("Waveform", OSCILLATOR_MODE_SINE, kNumOscillatorModes);
    GetParam(kWaveform)->SetDisplayText(0, "Sine"); // Needed for VST3, thanks plunntic

	GetParam(kFilterMode)->InitEnum("Filter Mode", Filter::FILTER_MODE_LOWPASS, Filter::kNumFilterModes);

	//============================================================================
	//  GUI COMPONENTS
	//============================================================================

    IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
    pGraphics->AttachPanelBackground(&COLOR_RED);

	// Waveform switch
    IBitmap waveformBitmap = pGraphics->LoadIBitmap(WAVEFORM_ID, WAVEFORM_FN, 4);
    pGraphics->AttachControl(new ISwitchControl(this, 50, 350, kWaveform, &waveformBitmap));

	// Filtermode Switch
	IBitmap filtermodeBitmap = pGraphics->LoadIBitmap(FILTERMODE_ID, FILTERMODE_FN, 3);
	pGraphics->AttachControl(new ISwitchControl(this, 50, 275, kFilterMode, &filtermodeBitmap));

	/// DISTORTION KNOB
    //initializes the bit map for the knob, used in the rotating/frames
    IBitmap knob2 = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames2);
    //attaches the knob to the GUI, specifies the parameters of the knob, and what it controls
    pGraphics->AttachControl(new IKnobMultiControl(this, kThresholdX, kThresholdY, kThreshold, &knob2));


	///ATTACK KNOB
    IBitmap knob3 = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames3);
    pGraphics->AttachControl(new IKnobMultiControl(this, kAttackX, kAttackY, kAttack, &knob3));

	///DECAY KNOB
    IBitmap knob4 = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames4);
    pGraphics->AttachControl(new IKnobMultiControl(this, kDecayX, kDecayY, kDecay, &knob4));
	  
	///SUSTAIN KNOB
    IBitmap knob5 = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames5);
    pGraphics->AttachControl(new IKnobMultiControl(this, kSustainX, kSustainY, kSustain, &knob5));

	///RELEASE KNOB
    IBitmap knob6 = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames6);
    pGraphics->AttachControl(new IKnobMultiControl(this, kReleaseX, kReleaseY, kRelease, &knob6));

	/// Knobs for filter cutoff and resonance
	IBitmap smallKnobBitmap = pGraphics->LoadIBitmap(KNOB_SMALL_ID, KNOB_SMALL_FN, 64);

		// Cutoff Knob:
		GetParam(kFilterCutoff)->InitDouble("Cutoff", 0.99, 0.01, 0.99, 0.001);
		GetParam(kFilterCutoff)->SetShape(2);
		pGraphics->AttachControl(new IKnobMultiControl(this, 100, 275, kFilterCutoff, &smallKnobBitmap));
		// Resonance Knob:
		GetParam(kFilterResonance)->InitDouble("Resonance", 0.01, 0.01, 1.0, 0.001);
		pGraphics->AttachControl(new IKnobMultiControl(this, 165, 275, kFilterResonance, &smallKnobBitmap));

	/// Knobs for filter envelope
		// Filter Attack knob
		GetParam(kFilterAttack)->InitDouble("Filter Env Attack", 0.01, 0.01, 10.0, 0.001);
		GetParam(kFilterAttack)->SetShape(3);
		pGraphics->AttachControl(new IKnobMultiControl(this, 100, 215, kFilterAttack, &smallKnobBitmap));

		// Filter Decay knob:
		GetParam(kFilterDecay)->InitDouble("Filter Env Decay", 0.5, 0.01, 15.0, 0.001);
		GetParam(kFilterDecay)->SetShape(3);
		pGraphics->AttachControl(new IKnobMultiControl(this, 165, 215, kFilterDecay, &smallKnobBitmap));

		// Filter Sustain knob:
		GetParam(kFilterSustain)->InitDouble("Filter Env Sustain", 0.1, 0.001, 1.0, 0.001);
		GetParam(kFilterSustain)->SetShape(2);
		pGraphics->AttachControl(new IKnobMultiControl(this, 230, 215, kFilterSustain, &smallKnobBitmap));

		// Filter Release knob:
		GetParam(kFilterRelease)->InitDouble("Filter Env Release", 1.0, 0.001, 15.0, 0.001);
		GetParam(kFilterRelease)->SetShape(3);
		pGraphics->AttachControl(new IKnobMultiControl(this, 295, 215, kFilterRelease, &smallKnobBitmap));

		// Filter envelope amount knob:
		GetParam(kFilterEnvelopeAmount)->InitDouble("Filter Env Amount", 0.0, -1.0, 1.0, 0.001);
		pGraphics->AttachControl(new IKnobMultiControl(this, 360, 215, kFilterEnvelopeAmount, &smallKnobBitmap));

	//finalizing GUI
    AttachGraphics(pGraphics);
    CreatePresets();


	//attaching the signals in the MidiReceiver to the MyFirstPlugin functions
	mMidiRec.noteOn.Connect(this, &MyFirstPlugin::onNoteOn);
    mMidiRec.noteOff.Connect(this, &MyFirstPlugin::onNoteOff);
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
        int velocity = mMidiRec.getLastVelocity();

		//if velocity is 0,nothing needs to be played
        if (velocity > 0) {
			//updates the frequency aka pitch aka current note being played
            LilJeffrey.setFrequency(mMidiRec.getLastFrequency());
            LilJeffrey.setMuted(false);
        } 
		else {
            LilJeffrey.setMuted(true);
        }            
		
		///notice here that the envelope value is also used now :)
		//beforeDist is the sample before the clipping (Vel/127 equates to volume)
	    beforeDistortion = LilJeffrey.nextSample() * mEnvGen.nextSample() *  (velocity / 127.0);


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

		//needed for filter envelope
		mFilter.setCutoffMod(mFilterEnvGen.nextSample() * filterEnvelopeAmount);

		//needed for the filter
		leftOutput[i] = rightOutput[i] = mFilter.process(LilJeffrey.nextSample() * mEnvGen.nextSample() * velocity / 127.0);
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
  LilJeffrey.setSampleRate(GetSampleRate());
  mEnvGen.setSampleRate(GetSampleRate());
  mFilterEnvGen.setSampleRate(GetSampleRate());
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
	  /*
    case kFrequency:
      LilJeffrey.setFrequency(GetParam(kFrequency)->Value());
      break;
      */
    case kThreshold:
		//sets the member value equal to the param value from the knob
         mThreshold = GetParam(kThreshold)->Value()/ 100.;
      break;

	case kAttack:
		//sets the member value equal to the param value from the knob
         mEnvGen.setStageValues(STAGE_ATTACK, (GetParam(kAttack)->Value())  );
      break;

	case kDecay:
		//sets the member value equal to the param value from the knob
         mEnvGen.setStageValues(STAGE_DECAY, (GetParam(kDecay)->Value())  );
      break;

	case kSustain:
		//sets the member value equal to the param value from the knob
         mEnvGen.setStageValues(STAGE_SUSTAIN, (GetParam(kSustain)->Value())  );
      break;

	case kRelease:
		//sets the member value equal to the param value from the knob
         mEnvGen.setStageValues(STAGE_RELEASE, (GetParam(kRelease)->Value())  );
      break;

	case kWaveform:
		LilJeffrey.setMode(static_cast<OscillatorMode>(GetParam(kWaveform)->Int()));
      break;

	case kFilterCutoff:
		mFilter.setCutoff(GetParam(paramIdx)->Value());
      break;

	case kFilterResonance:
		mFilter.setResonance(GetParam(paramIdx)->Value());
      break;

	case kFilterMode:
		mFilter.setFilterMode(static_cast<Filter::FilterMode>(GetParam(paramIdx)->Int()));
      break;

	case kFilterAttack:
         mFilterEnvGen.setStageValues(STAGE_ATTACK, (GetParam(kFilterAttack)->Value())  );
      break;

	case kFilterDecay:
         mFilterEnvGen.setStageValues(STAGE_DECAY, (GetParam(kFilterDecay)->Value())  );
	  break;

	case kFilterSustain:
         mFilterEnvGen.setStageValues(STAGE_SUSTAIN, (GetParam(kFilterSustain)->Value())  );
	  break;

	case kFilterRelease:
         mFilterEnvGen.setStageValues(STAGE_RELEASE, (GetParam(kFilterRelease)->Value())  );
	  break;

	case kFilterEnvelopeAmount:
		filterEnvelopeAmount = GetParam(paramIdx)->Value();
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