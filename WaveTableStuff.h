#include "Globals.h"

#ifndef WAVETABLESTUFF_H
#define	WAVETABLESTUFF_H

///need to search dsound.h to see if there's conflicting variable names or some shit?

//this struct holds information about a wavetable
struct WaveTable
{
	float *table;
	Int32   ID;
};



//defining the index values of where which type of wave will be in the wave table
// to kind of clarify whats going on when referring to specific indexes
//sine wave
#define WT_SIN 0
//saw wave 
#define WT_SAW 1
//square wave
#define WT_SQR 2
//triangle wave
#define WT_TRI 3

/*
class thing{
public:
	int stuff;
}*/


//this class will be initialized to hold all the pre-calculated waveforms
class WaveTableSet
{
public:

	
	//pointers to the respective wave tables
	float *wavSin;
	float *wavSqr;
	float *wavSaw;
	float *wavTri;
	

	
	//array of wavetables, that currently holds numTables amount of tables
	WaveTable *wavSet;
	//current number of wavetables in wavset
	Int32 numTables;

	///constructor
	//just initializes all pointers to zero basically
	WaveTableSet()
	{
		wavSin = 0;
		wavSqr = 0;
		wavSaw = 0;
		wavTri = 0;
		numTables = 4;
		wavSet= 0;
	}




	~WaveTableSet()
	{
		DestroyTables();
	}




	//kills all wavetables
	void DestroyTables()
	{
		//if wavset exists, loop through it and delete all the tables, and waveset itself
		if (wavSet)
		{
			for (Int32 i = 0; i < numTables; i++)
				delete wavSet[i].table;
			delete[] wavSet;
		}
		//setting wavset back to zero
		wavSet = 0;
		//setting numTables to the current num of tables aka 0
		numTables = 0;
	}


	//this function takes in an index to refer to the waveTableSet
	//and returns a pointer to the wavetable at that index
	float *GetWT(Int32 index)
	{
		float *pointerToTable = 0;

		//checking if that index is valid
		if ((index >= 0) && (index < numTables))
			pointerToTable = wavSet[index].table;

		if (pointerToTable)
			//returns pointer to requested wavetable
			return pointerToTable;
		//if that wt doesnt exist, returns pointer to sine wave
		return wavSin;
	}




    /// Initialize the default wavetables. 
	/// The length of wavetables is set by the synthParams itableLength member
	void Init()
	{
		wavSet = new WaveTable[numTables];


		//allocating the wavetables based on the standard global table length
		unsigned int length = Jeffrey.intTableLength+1;
		wavSin = new float[length];
		wavSaw = new float[length];
		wavSqr = new float[length];
		wavTri = new float[length];

        //defining the number of partials to be used in the
        // the additive synthesis part of calculating the
        // saw, square, and 
        #define PARTIALS 16

		//keeping track of all the phsIncr values for all 16 partials
		double phsIncrements[PARTIALS];
		//keeping track of all the phase values for all 16 partials
		double phsValues[PARTIALS];

		int partNum;
		int partMax = 1;

		//the phsIncr is 2pi/tableLength because the table is supposed
		//to be a single period aka phase 0-2pi
		phsIncrements[0] = twoPI / (double) Jeffrey.tableLength;
		phsValues[0] = 0.0f;



		//this loop populates the arrays of phsincr's and phase values
		for (partNum = 1; partNum < PARTIALS; partNum++)
		{
			//the increment of each partial is the base phsInc * an integer multiple
			phsIncrements[partNum] = phsIncrements[0] * (partNum+1);
			//all initial phases start at 0
			phsValues[partNum] = 0.0;

			//excluding the partials who's increment are > pi
			if (phsIncrements[partNum] < PI)
				partMax++;
		}
		


		//declaring/initializing the value and peak variables for each sum of parts wave
		double sawVal = 0.0;
		double sawMax = 0.0;
		double sqrVal;
		double sqrMax = 0.0;
		double triVal;
		double triMax = 0.0;


		//used in sigma approximation
		double sigK = PI / partMax;
		double sigN;
		double sigma;

		//value the partials are multiplied by (calculated for each partial)
		double amp;

		//initial sine-generated value to be added in the partial-summing part
		double value;

		//used in the calc of amp, starts as 2, because the first added partial is 
		//gets its amplitude divided by 2, increments by 1 for each partial
		double partRatio;

		Int16 index;

		///loops through all indexes for all wavetables
		for (index = 0; index < Jeffrey.intTableLength; index++)
		{

			value = sin(phsValues[0]);

			//setting the sin wavetable directly to the sin() value
			//since no sums of partials is needed
			wavSin[index] = (float) value;


			//incrementing the phase for the sine wave aka index 0
			phsValues[0] += phsIncrements[0];


			// These are calculated inside of the 
			sawVal = value;
			sqrVal = value;
			triVal = value;


			sigN = sigK;
			partRatio = 2;
			for (partNum = 1; partNum < partMax; partNum++)
			{
				
				// Adjustment with Lanczos sigma to minimize Gibbs phenomenon
				sigma = sin(sigN) / sigN; 
				//actually calculating amp value for each partial based on partRatio
				amp = sigma / partRatio;
				sigN += sigK;


				value = sin(phsValues[partNum]) * amp;


				//bitwise comparison between partNum and 1
				if (!(partNum & 1)) ///(odd partials)
				{
					sawVal += value;

					//square waves formed by adding only the odd partials
					sqrVal += value; 

					//triangle waves alsoonly add the odd partials
					triVal += ((cos(phsValues[partNum]) * sigma) / (partRatio * partRatio));
				}
				else ///(even partials)
					//even partials are subtracted to form saw waves
					sawVal -= value;  
				phsValues[partNum] += phsIncrements[partNum];
				partRatio += 1.0;
			}


			///the Max values keep track of the highest generated amplitude value
			///to be used for the normalizing the values 
			//fabs() finds the absolute value of the parameter passed
			//needed incase the max is a negative number 
			if (fabs(sawVal) > sawMax)
				sawMax = fabs(sawVal);

			if (fabs(sqrVal) > sqrMax)
				sqrMax = fabs(sqrVal);

			if (fabs(triVal) > triMax)
				triMax = fabs(triVal);


			//actually adding the samples into the three wave buffers aka wavetables
			wavSaw[index] = (float) sawVal;

			wavSqr[index] = (float) sqrVal;

			wavTri[index] = (float) triVal;
		}



		/// Normalizing/compressing summed values back down to [-1,1]
		//needed since summing sine waves results in amplitudes outside of the
		// [-1,1] range
		for (index = 0; index < Jeffrey.intTableLength; index++)
		{
			wavSaw[index] = wavSaw[index] / (float) sawMax;
			wavSqr[index] = wavSqr[index] / (float) sqrMax;
			wavTri[index] = wavTri[index] / (float) triMax;

		}

		//float copy[16384+1] = wavSin;

		// Set gaurd point for interpolation/round-up
		wavSin[Jeffrey.intTableLength] = wavSin[0];
		wavSaw[Jeffrey.intTableLength] = wavSaw[0];
		wavSqr[Jeffrey.intTableLength] = wavSqr[0];
		wavTri[Jeffrey.intTableLength] = wavTri[0];


		//setting the wavetables in the waveSet to the ones just calculated
		wavSet[WT_SIN].table = wavSin;
		wavSet[WT_SIN].ID = WT_SIN;
		wavSet[WT_SAW].table = wavSaw;
		wavSet[WT_SAW].ID = WT_SAW;
		wavSet[WT_SQR].table = wavSqr;
		wavSet[WT_SQR].ID = WT_SQR;
		wavSet[WT_TRI].table = wavTri;
		wavSet[WT_TRI].ID = WT_TRI;
	}

	
};




#endif	/* WAVETABLESTUFF_H */
