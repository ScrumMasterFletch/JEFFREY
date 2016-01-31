#ifndef __MYFIRSTPLUGIN__
#define __MYFIRSTPLUGIN__

#include "IPlug_include_in_plug_hdr.h"
#include "Oscillator.h"
#include "MIDIStuff.h"
#include "EnvelopeStuff.h"

class MyFirstPlugin : public IPlug
{
public:
  MyFirstPlugin(IPlugInstanceInfo instanceInfo);
  ~MyFirstPlugin();

  //these three methods are overriding methods of Iplug base
  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  void ProcessMidiMsg(IMidiMsg* pMsg);

private:
  double mFrequency;
  double mThreshold;
  void CreatePresets();
  Oscillator LilJeffrey;
  MIDIReceiver mMidiRec;
  EnvGen mEnvGen;

  //used in context with MIDI and EnvGen
  inline void onNoteOn(const int noteNumber, const int velocity) { mEnvGen.enterStage(STAGE_ATTACK); };
  inline void onNoteOff(const int noteNumber, const int velocity) { mEnvGen.enterStage(STAGE_RELEASE); };

};

#endif
