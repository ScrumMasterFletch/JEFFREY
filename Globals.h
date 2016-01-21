/* 
 * File:   Globals.h
 * Author: Austin
 *
 * Created on January 2, 2016, 4:38 PM
 */

#include <math.h>

#ifndef GLOBALS_H
#define	GLOBALS_H

#define PI 3.14159265358979
#define twoPI 6.28318530717958
#define sinv sin


/*JUST SOME TYPE ALIASING HERE THAT I THNK WILL HELP CLEAR UP 
WHATS GOING ON AND WHY EACH SIZE IS USED ETC*/
/// sample output type (16-bit for PCM)
typedef short SampleValue;
/// 8-bit data type
typedef char Int8;
/// 16-bit data type 
typedef short Int16;
/// 32-bit data type 
typedef int Int32;
/// 8-bit unsigned type
typedef unsigned char  Uint8;
/// 16-bit unsigned type
typedef unsigned short Uint16;
/// 32-bit unsigned type
typedef unsigned int   Uint32;






//============================================================================
//  Global synth object for easy attribute referral
//============================================================================


class GlobalJeffrey
{
public:
	/// Sample rate
	float sampleRate; //
	/// Maximum frequency (Nyquist limit, aka half of 
	float nyquist; //
	/// Sample rate as integer
	Int32 isampleRate; //
	/// multiplier to convert internal sample values into output values
	float sampleScale; //
	/// pre-calculated multipler for frequency to radians (twoPI/sampleRate)
	double frqRad; //
	/// table to convert pitch index into frequency
	float tuning[128];
	/// pre-calculated multipler for frequency to table index (tableLength/sampleRate)
	double frqToIndex;
	/// pre-calculated multipler for radians to table index (tableLength/twoPI)
	double radToIndex;
	/// wave table length
	double tableLength;
	/// wave table length as integer
	Int32  intTableLength;
	/// maximum phase increment for wavetables (ftableLength/2)
	double maxIncrWT;



	/// Constructor. The constructor for \p SynthConfig initializes
	/// member variables to default values by calling \p Init().
	/// In addition, the pitch-to-frequency table is filled with values
	/// for an equal-tempered scale.
	GlobalJeffrey()
	{
		Init();

		sampleScale = pow( 2.0, (16.0 - 1.0));

		int i;
		double frq = 13.75 * pow(2.0, 0.25); // C1 = A0*2^(3/12) = 16.35159...
		double two12 = pow(2.0, 1.0/12.0); // 2^(1/12) = 1.059463094...
		for (i = 0; i < 128; i++)
		{
			tuning[i] = (float) frq;
			frq *= two12;
		}

	}



	//initializes most of the object's variables
	void Init(Int32 sr = 44100, Int32 tl = 16384)
	{
		//setting sample rate to the standard but can br changed by calling init with a different value
		sampleRate = (float) sr;
		nyquist = sampleRate * 0.5;
		isampleRate = sr;
		frqRad = twoPI / (double) sampleRate;

		//setting table length to the standard but can br changed by calling init with a different value
		intTableLength = tl;
		tableLength = (double) tl;
		maxIncrWT = tableLength * 0.5;

		//calculating the ToIndex conversion multipliers 
		frqToIndex = tableLength / (double) sampleRate;
		radToIndex = tableLength / twoPI;
	}


	//Convert's pitch index (0-127) to actual float frequency value
	float GetFrequency(int pitch)
	{
		if (pitch < 0 || pitch > 127)
			return tuning[0] * pow(2.0, (double) pitch / 12.0);
		return tuning[pitch];
	}


};

//GLOBAL JEFFREY OBJECT JUST FOR EASY ACCESS TO PARAMETERS
//is declared in Globals.cpp again, had to for compiler reasons
extern GlobalJeffrey Jeffrey;


#endif	/* GLOBALS_H */
