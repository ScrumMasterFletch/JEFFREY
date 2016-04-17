#ifndef __MYFIRSTPLUGIN__
#define __MYFIRSTPLUGIN__

#include "IPlug_include_in_plug_hdr.h"
#include "Oscillator.h"
#include "MIDIStuff.h"
#include "EnvelopeStuff.h"
#include "Filter.h"
#include "VoiceManager.h"

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
  MIDIReceiver mMidiRec;

  //used for delay and reverb
  double delayTime;
  double delayAmount;
  int delayFrames;


  double delayBuffer[44100];
  double delayBuffer2[44100];
  double delayBuffer3[44100];
  double delayBuffer4[44100];
  double delayBuffer5[44100];
  double delayBuffer6[44100];
  double delayBuffer7[44100];


  int counter;
  int counter2;
  int counter3;
  int counter4;
  int counter5;
  int counter6;
  int counter7;


  VoiceManager LilJeffreyManager;

  ///removed to be replaced Voices/Voice manager
  //Oscillator LilJeffrey;
  //EnvGen mEnvGen;
  //Filter mFilter;
  //Oscillator mLFO;
  //double lfoFilterModAmount;
  //EnvGen mFilterEnvGen;
  //double filterEnvelopeAmount;

  /*
  //used in context with MIDI and EnvGen
  inline void onNoteOn(const int noteNumber, const int velocity) {
    mEnvGen.enterStage(STAGE_ATTACK);
    mFilterEnvGen.enterStage(STAGE_ATTACK);
  };

  inline void onNoteOff(const int noteNumber, const int velocity) {
    mEnvGen.enterStage(STAGE_RELEASE);
    mFilterEnvGen.enterStage(STAGE_RELEASE);
  };
  */
  //used to signal EnvGen
  //inline void onBeganEnvelopeCycle() { LilJeffrey.setMuted(false); }
  //inline void onFinishedEnvelopeCycle() { LilJeffrey.setMuted(true); }

};

#endif
