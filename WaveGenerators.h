/* 
 * File:   WaveGenerators.h
 * Author: Austin
 *
 * Created on January 14, 2016, 5:07 PM
 */
#include "Globals.h"

#ifndef WAVEGENERATORS_H
#define	WAVEGENERATORS_H


//=======================================================================
//  BASE(SINE) WAVE CLASS
//=======================================================================


class BaseWave
{
public:
	//each time the wave is sampled, phase is incremented by phaseIncr
	double phaseIncr;
	double phase;
	float frequency;

	//constructor
	BaseWave()
	{
		phaseIncr = 0;
		phase = 0;
		frequency = 440; //HZ
	}

	// Sets the frequency and calls Reset
	virtual void Init(int n, float v)
	{
		if (n > 0)
			SetFrequency(float(v));
		Reset(0);
	}



	// calculates and returns the next sample. Param 'in' is the volume passed in.
	virtual float Sample(float in)
	{
		//this actually creates the sample, gen is the value from the wave function
		return Gen() * in;
	}


	// Set the Frequency. Frequency doesnt actually get changed unless you call reset();
	inline void SetFrequency(float f)
	{
		frequency = f;
	}



	// Modulate the frequency AKA pitch, d is the HZ frequency value to modulate by
	virtual void Modulate(float d)
	{
		phaseIncr = (double)(frequency + d) * Jeffrey.frqRad;

		if (phaseIncr > PI)
			phaseIncr = PI;
	}

	// Getter for frequency
	inline float GetFrequency()
	{
		return frequency;
	}



    //Sets the phase back to zero and recalculates the phaseIncr
	virtual void Reset(float initPhs = 0)
	{
		phaseIncr = (double)frequency * Jeffrey.frqRad;

		if (phaseIncr > PI)
			phaseIncr = PI;
	}


	//phasewrap is what keeps the phase between 0 and 2pi
	inline double PhaseWrap(double phase)
	{
		while (phase >= twoPI)
			phase -= twoPI;

		while (phase < 0)
			phase += twoPI;

		return phase;
	}


	/// Generates the next sample. The sample amplitude is normalized to [-1,+1] range. 
	//aka The caller must apply any amplitude peak level multiplier.
	virtual float Gen()
	{
		phase = PhaseWrap(phase);

		//actual sine function call
		float out = sinv(phase);

		phase += phaseIncr;
		return out;
	}


	//always returns true for oscillators
	int IsFinished()
	{
		return 1;
	}
};

#define oneDivPI (1.0/PI)


//=======================================================================
//  SAWTOOTH WAVE CLASS
//=======================================================================


/// Sawtooth wave by direct calculation
class SawWave : public BaseWave
{
public:
	
	//phase for sawWaves is in between -1 and 1, not 0 to 2pi
	virtual void Reset(float initPhs = 0)
	{
		phaseIncr = (double)((2 * frequency) / Jeffrey.sampleRate);

		if (initPhs >= 0)
		{
			phase = (initPhs * oneDivPI) - 1;
			while (phase >= 1)
				phase -= 2;
		}
	}

	
	virtual float Gen()
	{
		float v = phase;
		//this is the equivalent of calling PhaseWrap for the basewave
		if ((phase += phaseIncr) >= 1)
			phase = -1;
		return v;
		
	}

	virtual void Modulate(float d)
	{
		double f = (double)(frequency + d);
		if (f < 0)
			f = -f;
		phaseIncr = (2 * f) / Jeffrey.sampleRate;
	}
};



#define twoDivPI (2.0/PI)


//=======================================================================
//  TRIANGLE WAVE CLASS
//=======================================================================


// Triangle wave by direct calculation
class TriWave : public BaseWave
{
public:

	//NOTE: phase varies from [-PI, PI] not [0, 2PI]
	virtual float Gen()
	{
		
		float triValue = (float)(phase * twoDivPI);

		//increment flips signs every pi radians
		if (triValue < 0)
			triValue = 1.0 + triValue;
		else
			triValue = 1.0 - triValue;

		//this is the phase wrapper for triangle waves
		if ((phase += phaseIncr) >= PI)
			phase -= twoPI;
		return triValue;
	}
};


//=======================================================================
//  SQUARE WAVE CLASS
//=======================================================================


//square wave by direct calculation aka dont wanna use as a standalone sound
class SqrWave : public BaseWave
{
private:
	double mid;
	double duty; //this allows for a variable duty cycle
	float ampMax;
	float ampMin;

public:
	SqrWave()
	{
		mid = PI;
		duty = 50.0; //standard unless changed
		ampMax = 1.0;
		ampMin = -1.0;
	}

	// Set the duty cycle. d is a percent (0-100)
	void inline SetDuty(float d)
	{
		duty = (double)d;
	}


	// Set the min/max amplitudes. pretty much always [-1,1]
	void inline SetMinMax(float amin, float amax)
	{
		ampMin = amin;
		ampMax = amax;
	}



	virtual void Init(int n, float v, float d)
	{
		if (n > 0)
		{
			SetFrequency(float(v));
			if (n > 1)
				SetDuty(d);
		}
		Reset();
	}

	// Initialize the square wave. f is frequency in Hz, duty = duty cycle (0-100)
	void InitSqr(float f, float duty)
	{
		SetDuty(duty);
		SetFrequency(f);
		Reset();
	}

	
	virtual void Reset(float initPhs = 0)
	{
		BaseWave::Reset(initPhs); //call to parent class' reset

		//the variable midpoint is calculated by multiplying the period by the duty/100
		mid = twoPI * (duty / 100.0);
	}

	
	virtual float Gen()
	{
		float v;

		//checking if the phase is less or greater than the midpoint
		if  (phase > mid){
			v = ampMin; //-1
		} 
		else{
		    v = ampMax; //1
		}

		if ((phase += phaseIncr) >= twoPI)
			phase -= twoPI;
		return v;
	}
};






#endif	/* WAVEGENERATORS_H */
