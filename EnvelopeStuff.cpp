#include "EnvelopeStuff.h"


//constructor: just initializes all the needed values
EnvGen::EnvGen(){
    minLevel = 0.0001; //needed so level wont be 0
    currStage = STAGE_OFF,
    currLevel = minLevel;
    levelMult = 1.0;
    sampleRate = 44100.0;
    currIndex = 0;
    nextStageIndex = 0; 

	//lengthss of each stage in seconds, EXCEPT SUSTAIN
    stageValues[STAGE_OFF] = 0.0;
    stageValues[STAGE_ATTACK] = 0.01;
    stageValues[STAGE_DECAY] = 0.5;
    stageValues[STAGE_SUSTAIN] = 0.1;  //USED FOR CURRLEVEL DURING SUSTAIN STAGE
    stageValues[STAGE_RELEASE] = 1.0;
}


///THIS CALCULATES THE MULTIPLIER THATS APPLIED TO CURRLEVEL BASED ON STAGE
//is either barely > or < 1
//ex: if we're in attack, its gonna be > 1, aka increasing the volume
void EnvGen::calculateMultiplier(double startLvl, double endLvl, unsigned long long lengthInSamples) {

    levelMult = 1.0 + (log(endLvl) - log(startLvl)) / (lengthInSamples);

}



///CHECKS IF WE ARE CURRENTLY IN THE CORRECT STAGE, THEN UPDATES CURRLEVEL
double EnvGen::nextSample() {

	///	THIS CHECKS IF THE ENVGEN NEEDS TO MOVE TO THE NEXT STATE, BUT ONLY IN THE
	/// CASES OF ATK, DEC, OR REL. OFF/SUSTAIN CHANGES ARE TRIGGERED BY EVENTS
	//checks if the current stage isnt off or sustain (both of which are indefinite)
    if ((currStage != STAGE_OFF) && (currStage != STAGE_SUSTAIN)) 
	{
		//if the current index == the next index, aka we need to enter a new stage:
        if (currIndex == nextStageIndex)
		{
			//declares a new stage to be passed to enterStage as the next stage
			int stageToBeEntered;

			switch(currStage)
			{
            case STAGE_ATTACK:
              stageToBeEntered = STAGE_DECAY;
                  break;

			case STAGE_DECAY:
              stageToBeEntered = STAGE_SUSTAIN;
                  break;

            case STAGE_RELEASE:
              stageToBeEntered = STAGE_OFF;
                  break;

            default:
                  break;
            }

			//progressing to the next stage determined by above switch
            enterStage(stageToBeEntered);
        }

		//finally updates level using the multiplier
        currLevel *= levelMult;
        currIndex++;
    }

	//returns the updated level
    return currLevel;
}



///CHANGES TO THE NEXT STATE, UPDATES CURRLEVEL AND LEVELMULT
void EnvGen::enterStage(int newStage) {
	//updates the current stage
    currStage = newStage;

	//resets the current index since we've entered a new stage
    currIndex = 0;


    if ((currStage == STAGE_OFF) || (currStage == STAGE_SUSTAIN)) 
	{
        nextStageIndex = 0;
    } 
	else //we're in atk, dec, or release
	{
		//calculates how many samples are in this stage
        nextStageIndex = stageValues[currStage] * sampleRate;
    }

	///SWITCH STATEMENT GETS CORRECT CURRLEVEL/LEVELMULT VALUES BASED ON STAGE
    switch (newStage) {
        case STAGE_OFF:
            currLevel = 0.0;
            levelMult = 1.0;
            break;


        case STAGE_ATTACK:
			//attack starts from the lowest level to 1
            currLevel = minLevel; //started from the bottom
            calculateMultiplier(currLevel, 1.0, nextStageIndex);
            break;



        case STAGE_DECAY:
			//after attack, we should have reached peak level
            currLevel = 1.0; //now we here

			//caclculating max of sustain-stage's value or min level
			double max;
			if(stageValues[STAGE_SUSTAIN] > minLevel)
			{
				max = stageValues[STAGE_SUSTAIN];
			}
			else max = minLevel;

            calculateMultiplier(currLevel, max, nextStageIndex);
            break;



        case STAGE_SUSTAIN:
            currLevel = stageValues[STAGE_SUSTAIN]; //only value in stageValues that isnt used as a duration
            levelMult = 1.0; //no level change here aka why its called sustain
            break;


        case STAGE_RELEASE:
            calculateMultiplier(currLevel, minLevel, nextStageIndex);
            break;

        default:
            break;
    }
}



void EnvGen::setSampleRate(double newSampleRate) {
    sampleRate = newSampleRate;
}


void EnvGen::setStageValues(int stage, double value){

	if((stage > 0) && (stage < 5))
	{
		stageValues[stage] = value;
	}

}
