#include "MyFirstPlugin.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include "Oscillator.h"


const int kNumPrograms = 5;

//enum used to keep track of params based on their index
enum EParams
{
  //these two were for the knobs
  //kFrequency = 0,
  //kThreshold = 1,
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

enum ELayout2
{
  kThresholdX = 500,
  kThresholdY = 300,
  kKnobFrames2 = 128
};


/*
MyFirstPlugin::MyFirstPlugin(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mFrequency(1.)
{
  TRACE;
  
  //==============================================================================
  //  PARAMETERS
  ///Parameters can either be double, enum, int, or bool e.g. ->InitBool(...); etc
  //each type of param expects different parameters in the init() call

  //adding a param with type(double), name, defaultVal, minVal, maxVal, step, label
  ///this is defines the index as kFrequency from the Eparams enum
  GetParam(kFrequency)->InitDouble("Frequency", 440.0, 50.0, 7900.0, 0.01, "Hz");
  GetParam(kFrequency)->SetShape(2.);


  ///THRESHOLD
  GetParam(kThreshold)->InitDouble("Threshold", 0.01, 0.01, 100.0, 0.01, "%");
  GetParam(kThreshold)->SetShape(1.);


  //essentially instantiating the GUI object
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  //pGraphics->AttachPanelBackground(&COLOR_TRANSPARENT);    //basic color background
  pGraphics->AttachBackground(BACKGROUND_ID, BACKGROUND_FN); //star wars background



  ///FIRST KNOB
  //
  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kFrequencyX, kFrequencyY, kFrequency, &knob));



  /// DECLARING/INITIALIZING SECOND KNOB=============================================================
  //initializes the bit map for the knob, used in the rotating/frames
  IBitmap knob2 = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  //attaches the knob to the GUI, specifies the parameters of the knob, and what it controls
  pGraphics->AttachControl(new IKnobMultiControl(this, kThresholdX, kThresholdY, kThreshold, &knob2));


  //kind of universal final GUI init call
  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  //MakeDefaultPreset((char *) "-", kNumPrograms);
  CreatePresets();
}
*/


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
MyFirstPlugin::MyFirstPlugin(IPlugInstanceInfo instanceInfo)
    :   IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo) {
    TRACE;

    IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
    pGraphics->AttachPanelBackground(&COLOR_RED);
    AttachGraphics(pGraphics);
    CreatePresets();
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
void MyFirstPlugin::ProcessDoubleReplacing(
    double** inputs,
    double** outputs,
    int nFrames)
{
    // Mutex is already locked for us.

    double *leftOutput = outputs[0];
    double *rightOutput = outputs[1];

    for (int i = 0; i < nFrames; ++i) {
        mMidiRec.advance();
        int velocity = mMidiRec.getLastVelocity();
        if (velocity > 0) {
            LilJeffrey.setFrequency(mMidiRec.getLastFrequency());
            LilJeffrey.setMuted(false);
        } else {
            LilJeffrey.setMuted(true);
        }
        leftOutput[i] = rightOutput[i] = LilJeffrey.nextSample() * velocity / 127.0;
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
  LilJeffrey.setSampleRate(GetSampleRate());
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

  /*  NEEDED IN ITERATION WITH 2 KNOBS

  //needs to modify all params
  switch (paramIdx)
  {
    case kFrequency:
      LilJeffrey.setFrequency(GetParam(kFrequency)->Value());
      break;

    case kThreshold:
         mThreshold = GetParam(kThreshold)->Value() / 100.;
      break;

    default:
      break;
  }
  */
}



/*
void MyFirstPlugin::CreatePresets() {

  //makepreset is an IplugBase function
  //takes input for each param in order(Eparam), so thats why only 1 argument here, only one param
  MakePreset("clean", 100.0);
  MakePreset("slightly distorted", 80.0);
  MakePreset("woooo", 40.0);
  MakePreset("waaaa", 20.0);
  MakePreset("buzzz!!!", 0.01);
}*/

/*
void MyFirstPlugin::CreatePresets() {
  MakePreset("clean", 440.0);
}*/



void MyFirstPlugin::CreatePresets() {
    MakeDefaultPreset((char *) "-", kNumPrograms);
}



///this is an overwrite of the IPlug function, gets called every time a MIDI message is received by VST
//just calls onMessageReceived with the message 
void MyFirstPlugin::ProcessMidiMsg(IMidiMsg* pMsg) {
    mMidiRec.onMessageReceived(pMsg);
}