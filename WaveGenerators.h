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
	double phaseIncr;
	double phase;
	float frq;

	BaseWave()
	{
		phaseIncr = 0;
		phase = 0;
		frq = 440;
	}

	/// Initialize the oscillator.
	virtual void Init(int n, float v)
	{
		if (n > 0)
			SetFrequency(float(v));
		Reset(0);
	}

	/// Return the next sample. Param 'in' is the volume passed in.
	virtual float Sample(float in)
	{
		return Gen() * in;
	}

	/// Set the Frequency. Frequency doesnt actually get changed unless you call reset();
	inline void SetFrequency(float f)
	{
		frq = f;
	}

	/// Modulate the oscillator frequency.  This forces recalculation of the
	/// phase increment by adding the argument to the last set frequency value.
	/// @param d delta frequency in Hz
	virtual void Modulate(float d)
	{
		phaseIncr = (double)(frq + d) * Jeffrey.frqRad;
		if (phaseIncr > PI)
			phaseIncr = PI;
	}

	/// Get the frequency.
	inline float GetFrequency()
	{
		return frq;
	}



	/// Reset the oscillator. The phase increment is calculated based on the 
	/// last set frequency value. The phase argument indicates the next phase.
	/// When set to 0, the oscillator is reset to the initial conditions. Values
	/// greater than 0 cause calculation of the appropriate starting sample.
	/// Values less than zero cause the phase to remain unchanged.
	virtual void Reset(float initPhs = 0)
	{
		phaseIncr = (double)frq * Jeffrey.frqRad;
		if (phaseIncr > PI)
			phaseIncr = PI;
		if (initPhs >= 0)
			phase = initPhs;
	}



	inline double PhaseWrap(double phase)
	{
		while (phase >= twoPI)
			phase -= twoPI;
		while (phase < 0)
			phase += twoPI;
		return phase;
	}


	/// Generate the next sample. The sample amplitude is normalized to [-1,+1] range. 
	//aka The caller must apply any amplitude peak level multiplier.
	virtual float Gen()
	{
		phase = PhaseWrap(phase);
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
	
	virtual void Reset(float initPhs = 0)
	{
		phaseIncr = (double)((2 * frq) / Jeffrey.sampleRate);
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
		if ((phase += phaseIncr) >= 1)
			phase = -1;
		return v;
		
	}

	virtual void Modulate(float d)
	{
		double f = (double)(frq + d);
		if (f < 0)
			f = -f;
		phaseIncr = (2 * f) / Jeffrey.sampleRate;
	}
};



#define twoDivPI (2.0/PI)


//=======================================================================
//  TRIANGLE WAVE CLASS
//=======================================================================


/// Triangle wave by direct calculation
class TriWave : public BaseWave
{
public:

	//NOTE: phase varies from [-PI, PI] not [0, 2PI]
	virtual float Gen()
	{
		//float triValue = (float)(1 + (2 * fabs(phase - PI) / PI);
		float triValue = (float)(phase * twoDivPI);
		if (triValue < 0)
			triValue = 1.0 + triValue;
		else
			triValue = 1.0 - triValue;
		if ((phase += phaseIncr) >= PI)
			phase -= twoPI;
		return triValue;
	}
};


//=======================================================================
//  SQUARE WAVE CLASS
//=======================================================================


/// Square wave by direct calculation. This is fast but not
/// bandwidth limited and should only be used for LFO effects,
/// not audio. This has a settable min/max so that it can toggle
/// from 0/1 as well as -1/+1, or any other pair of values
class SqrWave : public BaseWave
{
private:
	double midPoint;
	double dutyCycle;
	float ampMax;
	float ampMin;

public:
	SqrWave()
	{
		midPoint = PI;
		dutyCycle = 50.0;
		ampMax = 1.0;
		ampMin = -1.0;
	}

	/// Set the duty cycle. The duty cycle is specified in a percent
	/// of the period (e.g. 50 = half of period is on).
	/// @param d duty cycle (0-100)
	void inline SetDutyCycle(float d)
	{
		dutyCycle = (double)d;
	}


	/// Set the min/max amplitudes. Typically an oscillator is normalized
	/// to the [-1,+1] range. The square wave can be set to toggle between
	/// any two values so that it functions as a gate signal.
	void inline SetMinMax(float amin, float amax)
	{
		ampMin = amin;
		ampMax = amax;
	}

	// Fo, Duty%
	virtual void Init(int n, float v, float d)
	{
		if (n > 0)
		{
			SetFrequency(float(v));
			if (n > 1)
				SetDutyCycle(d);
		}
		Reset();
	}

	/// Initialize the square wave. f is frequency in Hz, duty = duty cycle (0-100)
	void InitSqr(float f, float duty)
	{
		SetDutyCycle(duty);
		SetFrequency(f);
		Reset();
	}

	
	virtual void Reset(float initPhs = 0)
	{
		BaseWave::Reset(initPhs);
		midPoint = twoPI * (dutyCycle / 100.0);
	}

	
	virtual float Gen()
	{
		float v = (phase > midPoint) ? ampMin : ampMax;
		if ((phase += phaseIncr) >= twoPI)
			phase -= twoPI;
		return v;
	}
};






#endif	/* WAVEGENERATORS_H */
