
#pragma once
#ifndef __ENVELOPESTUFF__
#define __ENVELOPESTUFF__


#include <cmath>

class EnvGen {
public:

#define STAGE_OFF 0
#define STAGE_ATTACK 1
#define STAGE_DECAY 2
#define STAGE_SUSTAIN 3
#define STAGE_RELEASE 4
#define NUM_STAGES 5


    void enterStage(int newStage);
    double nextSample();
    void setSampleRate(double newSampleRate);
    inline int getCurrentStage() const { return currStage; };
    double minLevel;
	EnvGen();
	void setStageValues(int stage, double value);
    
private:
    int currStage;
    double currLevel;
    double levelMult; //what level is multiplied by during current env-stage
    double sampleRate;
    double stageValues[NUM_STAGES];
    void calculateMultiplier(double startLvl, double endLvl, unsigned long long lengthInSamples);
    unsigned long long currIndex; //current index in current stage
    unsigned long long nextStageIndex; //the end of the current stage, signals to move to the next stage


};


#endif