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


	//============================================================================
	//  GUI COMPONENTS
	//============================================================================
    IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
    pGraphics->AttachPanelBackground(&COLOR_RED);


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
zeros if the host hasn�t connected them
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