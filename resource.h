/* RESOURCE.H:
specifies various properties of the plugin
- plugin name, manufacturer name
- valid channel i/o / side-chain
- plugin unique ID, manufacturer unique ID
- version number
- plugin type: instrument/effect/midi-controlled effect
- plugin category (VST3/RTAS)
- plugin does chunks or not
- specifies resource IDs and paths for each unique PNG image you
need to use in a control

*/
#define PLUG_MFR "Hunter"
#define PLUG_NAME "FilterCYAN"

#define PLUG_CLASS_NAME MyFirstPlugin

#define BUNDLE_MFR "Austin"
#define BUNDLE_NAME "MyFirstPlugin"

#define PLUG_ENTRY MyFirstPlugin_Entry
#define PLUG_VIEW_ENTRY MyFirstPlugin_ViewEntry

#define PLUG_ENTRY_STR "MyFirstPlugin_Entry"
#define PLUG_VIEW_ENTRY_STR "MyFirstPlugin_ViewEntry"

#define VIEW_CLASS MyFirstPlugin_View
#define VIEW_CLASS_STR "MyFirstPlugin_View"

// Format        0xMAJR.MN.BG - in HEX! so version 10.1.5 would be 0x000A0105
#define PLUG_VER 0x00010000
#define VST3_VER_STR "1.0.0"

// 4 chars, single quotes. At least one capital letter
#define PLUG_UNIQUE_ID 'Ipef'
// make sure this is not the same as BUNDLE_MFR
#define PLUG_MFR_ID 'Acme'

// ProTools stuff

#if (defined(AAX_API) || defined(RTAS_API)) && !defined(_PIDS_)
  #define _PIDS_
  const int PLUG_TYPE_IDS[2] = {'EFN1', 'EFN2'};
  const int PLUG_TYPE_IDS_AS[2] = {'EFA1', 'EFA2'}; // AudioSuite
#endif

#define PLUG_MFR_PT "Austin\nAustin\nAcme"
#define PLUG_NAME_PT "MyFirstPlugin\nIPEF"
#define PLUG_TYPE_PT "Effect"
#define PLUG_DOES_AUDIOSUITE 1

/* PLUG_TYPE_PT can be "None", "EQ", "Dynamics", "PitchShift", "Reverb", "Delay", "Modulation", 
"Harmonic" "NoiseReduction" "Dither" "SoundField" "Effect" 
instrument determined by PLUG _IS _INST
*/

  ///pretty sure our plugin will be classified as an instrument

//#define PLUG_CHANNEL_IO "1-1 2-2"
#if (defined(AAX_API) || defined(RTAS_API)) 
#define PLUG_CHANNEL_IO "1-1 2-2"
#else
// no audio input. mono or stereo output
#define PLUG_CHANNEL_IO "0-1 0-2"
#endif

#define PLUG_LATENCY 0
#define PLUG_IS_INST 1

// if this is 0 RTAS can't get tempo info
#define PLUG_DOES_MIDI 1

///do we need chunks??
#define PLUG_DOES_STATE_CHUNKS 0


///================================================================
// Unique IDs for each image resource.
#define KNOB_ID 101
#define BACKGROUND_ID 102
#define WAVEFORM_ID 103
#define KNOB_SMALL_ID 104
#define	FILTERMODE_ID 105


///================================================================
// Image resource locations for this plug.
#define KNOB_FN "resources/img/cyan_knob.png"
#define BACKGROUND_FN "resources/img/background.png"
#define WAVEFORM_FN   "resources/img/waveform.png"
#define KNOB_SMALL_FN "resources/img/cyan_knob.png"
#define FILTERMODE_FN "resources/img/filtermode.png"

// GUI default dimensions
#define GUI_WIDTH 598
#define GUI_HEIGHT 448

// on MSVC, you must define SA_API in the resource editor preprocessor macros as well as the c++ ones
#if defined(SA_API) && !defined(OS_IOS)
#include "app_wrapper/app_resource.h"
#endif

// vst3 stuff
#define MFR_URL "www.olilarkin.co.uk"
#define MFR_EMAIL "spam@me.com"
#define EFFECT_TYPE_VST3 "Instrument|Synth"

/* "Fx|Analyzer"", "Fx|Delay", "Fx|Distortion", "Fx|Dynamics", "Fx|EQ", "Fx|Filter",
"Fx", "Fx|Instrument", "Fx|InstrumentExternal", "Fx|Spatial", "Fx|Generator",
"Fx|Mastering", "Fx|Modulation", "Fx|PitchShift", "Fx|Restoration", "Fx|Reverb",
"Fx|Surround", "Fx|Tools", "Instrument", "Instrument|Drum", "Instrument|Sampler",
"Instrument|Synth", "Instrument|Synth|Sampler", "Instrument|External", "Spatial",
"Spatial|Fx", "OnlyRT", "OnlyOfflineProcess", "Mono", "Stereo",
"Surround"
*/
