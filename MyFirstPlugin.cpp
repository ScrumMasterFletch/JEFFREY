#include "MyFirstPlugin.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
  kGain = 0,
  kPan = 0,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kGainX = 100,
  kGainY = 100,
  kKnobFrames = 60
};

enum ELayou2
{
  k2Width = GUI_WIDTH,
  k2Height = GUI_HEIGHT,

  k2GainX = 200,
  k2GainY = 100,
  k2KnobFrames = 60
};
enum ELayou3
{
  k3Width = GUI_WIDTH,
  k3Height = GUI_HEIGHT,

  k3GainX = 150,
  k3GainY = 200,
  k3KnobFrames = 60
};

MyFirstPlugin::MyFirstPlugin(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mPan(1.), mGain(1.)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kGain)->InitDouble("Gain", 50., 0., 100.0, 0.01, "%");
  GetParam(kGain)->SetShape(2.);
   GetParam(kPan)->InitDouble("Pan", 50., 0., 100.0, 0.01, "%");
  GetParam(kPan)->SetShape(2.);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachPanelBackground(&COLOR_GREEN);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kGainX, kGainY, kGain, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, k2GainX, k2GainY, kGain, &knob));
  pGraphics->AttachControl(new IKnobMultiControl(this, k3GainX, k3GainY, kPan, &knob));
  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

MyFirstPlugin::~MyFirstPlugin() {}

void MyFirstPlugin::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];

  for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
  {
    *out1 = *in1 * mGain;
    *out2 = *in2 * mGain;
  }
}

void MyFirstPlugin::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void MyFirstPlugin::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kGain:
      mGain = GetParam(kGain)->Value() / 100.;
      break;


    default:
      break;
  }	
  switch (paramIdx)
  {
   case kPan:
      mPan = GetParam(kPan)->Value() / 100.;
      break;


    default:
      break;
  }
}
