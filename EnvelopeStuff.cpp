#include "EnvelopeStuff.h"

//now all envelope generators will have the same sample rate
double EnvGen::sampleRate = 44100.0;

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

//constructor: just initializes all the needed values
EnvGen::EnvGen(){
    //minLevel = 0.0001; //needed so level wont be 0
	minLevel = 0.001;
    currStage = STAGE_OFF,
    currLevel = minLevel;
    levelMult = 1.0;
    //sampleRate = 44100.0; //declared as static at top of file
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
	if (currStage == STAGE_OFF) return minLevel;
	else return currLevel;
}



///CHANGES TO THE NEXT STATE, UPDATES CURRLEVEL AND LEVELMULT
void EnvGen::enterStage(int newStage) {

	if (currStage == newStage) return;
    if (currStage == STAGE_OFF) 
	{
       beganEnvelopeCycle();
    }
    if (newStage == STAGE_OFF) 
	{
       finishedEnvelopeCycle();
    }


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



///this setter is used in OnParamChange in the main program to set these values from the knobs
void EnvGen::setStageValues(int stage, double value) {

	//here is where the stageValues array is actually updated, the rest of the 
	//code below is updating the multipliers and nextStageIndex so the new
	//value will affect the current envelope, not just the next one generated
    stageValues[stage] = value;

	//"if we're in the current stage thats getting changed"
    if (stage == currStage) {

        if((currStage == STAGE_ATTACK) || (currStage == STAGE_DECAY) || (currStage == STAGE_RELEASE)) {
            double nextLevelValue;
			//switch statement handling each state case
            switch (currStage) 
			{
                case STAGE_ATTACK:
                    nextLevelValue = 1.0;
                    break;

                case STAGE_DECAY:
					double max;
					if(stageValues[STAGE_SUSTAIN] > minLevel)
			        {
				       max = stageValues[STAGE_SUSTAIN];
			        }
			        else max = minLevel;
                    break;

                case STAGE_RELEASE:
                    nextLevelValue = minLevel;
                    break;

                default:
                    break;
            }

            //calculating where the generator is in the current stage:
            double currStageProgress = (currIndex + 0.0) / nextStageIndex;

            //calculating whats left of the current stage
            double restOfStage = 1.0 - currStageProgress;

			//calculating the number of samples left in the stage now that the param has changed
            unsigned long long samplesLeftInStage = restOfStage * value * sampleRate;

			//updating nextStageIndex with the new samplesLeft value
            nextStageIndex = currIndex + samplesLeftInStage;

			///finally recalculating the multiplier based on the new samplesLeft  and nextLevel values
            calculateMultiplier(currLevel, nextLevelValue, samplesLeftInStage);
        } 
		else //aka current state is either Stage_off or Stage_sustain
		{
			if(currStage == STAGE_SUSTAIN) {
            currLevel = value;
		}


        }
    }

	///checks the case that we're in decay, and sustain gets changed, so the end value needs to be updated
	if ((currStage == STAGE_DECAY) && (stage == STAGE_SUSTAIN)) {

    unsigned long long samplesLeft = nextStageIndex - currIndex;

	double max;
	if(stageValues[STAGE_SUSTAIN] > minLevel)
	{
	   max = stageValues[STAGE_SUSTAIN];
	}
	else max = minLevel;

	//updating the multiplier so we decay correctly to the new value
    calculateMultiplier(currLevel, max, samplesLeft);
    }

}

void EnvGen::reset() {
    currStage = STAGE_OFF;
    currLevel = minLevel;
    levelMult = 1.0;
    currIndex = 0;
    nextStageIndex = 0;
}