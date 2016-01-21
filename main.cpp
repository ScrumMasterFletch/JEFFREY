#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include "Globals.h" //
#include "WaveFile.h"//
#include "WaveGenerators.h" //
#include "DirectSoundStuff.h"//
#include "WaveTableStuff.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
//#include <dsound.h>
//#include <dinput.h>
//#include "Test.h"
//#include "Globals.h"


//linking stuff
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

using namespace std;



//global set of wave tables so the sine wave can be addressed anywhere via
//wtSet[0] etc
WaveTableSet wtSet;



// Window Procedure called by Windows
LRESULT CALLBACK WindowProcedure
    (HWND hwnd, unsigned int message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        
        // check if window is being destroyed 
        case WM_DESTROY:
        {    
            PostQuitMessage (0);
            return 0;
        }    
        
        // Check if the window is being closed.
        case WM_CLOSE:
	    {
		    PostQuitMessage(0);		
		    return 0;
	    }

	// All other messages pass to the message handler in the system class.
	    default:
	    {
		    return DefWindowProc (hwnd, message, wParam, lParam );
	    }

    }
    
}




//===========================================================================
//  MAIN PROGRAM(WIN32 APP OBVIOUSLY)
//===========================================================================
int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hPrevInst,
            char * cmdParam, int cmdShow)
//int main(int argc, char *argv[])
{
	//initializing the global synth parameters object Jeffrey
	Jeffrey.Init();

	//initializing the wavetableset, aka generating the waves
	wtSet.Init();

	


	//initilaizing some sound variables
	int pitch = 60;  // Middle c
    float duration = 6;
	float volume;
    Uint16 channels = 1;
	double phase = 0;

	///the jeffrey object here is a global object containing some global parameters
    //looking up the frequency based on the pitch
    //frequency is how often the function repeats per second
    //float frequency = Jeffrey.GetFrequency(pitch);
	float frequency = 220;
    float Bb = Jeffrey.GetFrequency(57);


    //phase increment is 2pi/(sample rate / (1/freq))
    //the frequency aka pitch of a note can change
    double phaseIncr = Jeffrey.frqRad * frequency;
        

    //calculating total samples to be created 
    Uint32 totalSamples = (Uint32) ((Jeffrey.sampleRate * duration) + 0.5);
        

		
    //declaring the buffer to store the samples
	Int16* sampleBuffer = new Int16[totalSamples];
    //Int16 sampleBuffer [44100]; //use this line after changing dur to 1 to be able to see inside array values
        
        
        

        
    //declaring the sample, aka whats going to be written to the wave
    SampleValue sample;

    //whats used for calculation and is gonna be scaled back up to 16-int
    //aka this is the "compressed" sample
    float compSample;

        


/*Maybe split up linear vs exponential into functions? or just use exponential
since more popular? have an option on GUI to pick linear vs exponential? when
 i say linear vs exponential here, im talking about the growth rate of the
 attack and decay*/     
        


        /*//envelope variables(only attack/decay envelope for right meow)
     //NEED TO EVENTUALLY BE EXTENDED INTO ADSR ENVELOPE AKA ADD DECAY/SUSTAIN
        float attackRate = .5;
        float decayRate = 1;
        float envInc;
        float peakAmp = 1.0;
        Uint32 attackTime = (Uint32) (attackRate * Jeffrey.sampleRate);
        Uint32 decayTime = (Uint32) (decayRate * Jeffrey.sampleRate);
        Uint32 decayStart = totalSamples - decayTime;
        
        if (attackTime > 0){
            envInc = (float) peakAmp / attackTime;
        }
        
        
        //exponential attack/decay variables
        //(this was the previous site of the worst rounding error in history)
        //(you think im joking, im not)
        double range = (double) peakAmp / Jeffrey.sampleRate;
        //cout << "Range = " << range << "\n";
        double expIncr = (double) 1 / attackTime; 
        //cout << expIncr;
        double expX = 0;
        double expB = 2;
        */

    //set this to 1 here to guarantee playback during DirectSound fiasco,
	//needs to be start at 0 for any of the envelope stuff
    volume = 1;
        
        

    //testing WaveFile class(see wavefile.h/cpp)
    WaveFile wav;
    char filename [] = "out2.wav";



    //variables for wub-wub
	float numOfWubs = 12;
	float applyRate = 100; 
    float wubWubIncrement = 1 / (((duration * Jeffrey.sampleRate) / numOfWubs) / applyRate);



	//creating a SINE wave to be used in the loop
	BaseWave SineWave;
	SineWave.Init(1, frequency);
	SineWave.Reset();

	

	//creating a SAW wave to be used in the loop
	SawWave Morty;
	Morty.Init(1, frequency);
	Morty.Reset();



	//creating a SAW wave to be used in the loop
	TriWave Rick;
	Rick.Init(1, frequency);
	Rick.Reset();

	//creating a SAW wave to be used in the loop
	SqrWave Dernay;
	Dernay.InitSqr(frequency, 50);


	//stuff for testing additive synthesis
	double phsIncr[15];
    phsIncr[0] = Jeffrey.frqRad * frequency;
	phsIncr[1] = Jeffrey.frqRad * frequency * 2;
	phsIncr[2] = Jeffrey.frqRad * frequency * 3;
	phsIncr[3] = Jeffrey.frqRad * frequency * 4;
	phsIncr[4] = Jeffrey.frqRad * frequency * 5;
	phsIncr[5] = Jeffrey.frqRad * frequency * 6;
	phsIncr[6] = Jeffrey.frqRad * frequency * 7;
	phsIncr[7] = Jeffrey.frqRad * frequency * 8;
	phsIncr[8] = Jeffrey.frqRad * frequency * 9;
	phsIncr[9] = Jeffrey.frqRad * frequency * 10;
	phsIncr[10] = Jeffrey.frqRad * frequency * 11;
	phsIncr[11] = Jeffrey.frqRad * frequency * 12;
	phsIncr[12] = Jeffrey.frqRad * frequency * 13;
	phsIncr[13] = Jeffrey.frqRad * frequency * 14;
	phsIncr[14] = Jeffrey.frqRad * frequency * 15;



	double phase1[15];
	phase1[0] = 0;
	phase1[1] = 0;
	phase1[2] = 0;
	phase1[3] = 0;
	phase1[4] = 0;
	phase1[5] = 0;
	phase1[6] = 0;
	phase1[7] = 0;
	phase1[8] = 0;
	phase1[9] = 0;
	phase1[10] = 0;
	phase1[11] = 0;
	phase1[12] = 0;
	phase1[13] = 0;
	phase1[14] = 0;

	float max=0;


	///===================================================
	// TESTING WAVETABLES STUFF
	///====================================================
	//wtPointer is now a reference to the sine wave
	float *wtPointer;
	wtPointer = wtSet.GetWT(WT_TRI);
	double indexIncr = 0;
	double index = 0;

	indexIncr = double(frequency) * Jeffrey.frqToIndex;
			index = 0;
///////////////////////////////////////////////////////////////////////////////        
//LOOP THAT ACTUALLY CALCULATES ALL SAMPLES AND WRITES THEM TO THE BUFFER======
///////////////////////////////////////////////////////////////////////////////        
        //actually writing to the wave file
	for (long n = 0; n < totalSamples; n++)
	{

		//====================================================================
		//ENVELOPE STUFF, SCATTERED TESTING THINGS HERE, NEEDS TO BECOME CLASS
		//====================================================================

		/*
           /// MESSING AROUND WITH VOLUME TO WUBWUBWUBWUB
		   //resets volume to 0 at end of wub for next wub
            if((n % (int) (totalSamples/numOfWubs) == 0)){
                volume = 0;
               
            }

			//every applyRate samples, volume is incremented
			//doesnt have to be incremented every sample because no audible difference
			if ((n % (int) applyRate) == 0)
			{
               if (volume < 1)
                   volume = volume + (wubWubIncrement);
               else volume = 1;
			}*/
            
            
            /*
            //STANDARD LINEAR envelope loop stuff=============================
            if ((n < attackTime) || (n > decayStart)){
                volume += envInc;
            }
            else if (n == attackTime){
                volume = peakAmp;
            }
            else if (n == decayStart){
                envInc = (-volume)/decayTime;
            }*/

			///exponential wasnt working in VisualStudio last time i checked, round off error
			///only played a value of 0 aka no sound
            /*
            //EXPONENTIAL envelope loop stuff=================================
            if ((n < attackTime) || (n > decayStart)){
                volume = 1 * pow(expX, expB);
                expX += expIncr; 
                
            }
            else if (n == decayStart){
                expIncr = (double) (-1)/decayTime; 
                expX = 1;
            }*/
            
            /*if(n % 1000 == 0)
                cout <<"v = " << volume << " X = "  << expX << "\n";*/
		    

		    ///STARRRRRRR WARRRRRRRRRRRRRRRRRRRRRRRRSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
			/*
			//STAR WARSSSSSSSSSSSSSSSSSSSSS
			if (n == 29400){
			   Morty.SetFrequency(Jeffrey.GetFrequency(53));
			   Morty.Reset();
			}

			if (n == 88200){
			   //SineWave.Modulate((Jeffrey.GetFrequency(55) - Jeffrey.GetFrequency(53)));
				Morty.SetFrequency(Jeffrey.GetFrequency(55));
			   Morty.Reset();
			}

			if (n == 132300){
			   //SineWave.Modulate((Jeffrey.GetFrequency(56) - Jeffrey.GetFrequency(55)));
				Morty.SetFrequency(Jeffrey.GetFrequency(56));
			   Morty.Reset();
			}

			if (n == 139650){
			   Morty.SetFrequency(466.16);
			   Morty.Reset();
			}

			if (n == 147000){
			   Morty.SetFrequency(Jeffrey.GetFrequency(56));
			   Morty.Reset();
			}

			if (n == 205800){
			   Morty.SetFrequency(Jeffrey.GetFrequency(48));
			   Morty.Reset();
			}*/




            //==================================================================== 
            //  WAVES AND ACTUAL SAMPLE CALCULATION AND RESCALING TO [-32K, 32K]
		    //====================================================================
            ///SINE WAVE===============================================
            //each sample is calculated by sample = volume * sin(phase)
            //volume is constant in this example because no envelope is used
            //this call is essentially => compSample = volume * sinv(phase);
            //compSample = SineWave.Sample(volume);



            ///SAWTOOTH WAVE===========================================
            //compSample = volume * ((phase/PI)-1);
			//compSample = Morty.Sample(volume);
            //
			


			///TRIANGLE WAVE===========================================
			//compSample = Rick.Sample(volume);
            //
            


		    ///SQUARE WAVE===========================================
			//compSample = Dernay.Sample(volume);
            //

			/*
            ///ADDITIVE SYNTHESIS SINE WAVE REAL QUICK
            //=======================================
		    float value = 0;
			
		    for (int p = 0; p <15; p++){
			    value += sinv(phase1[p]) / (p+1);
		        if ((phase1[p] += phsIncr[p]) >= twoPI)
		        phase1[p] -= twoPI;
			}
	
			if (value > max)
				max = value;

			compSample = volume * (value/1.76);*/

			///===================================================
	        // TESTING WAVETABLES STUFF
	        ///====================================================

			

			if (index >= Jeffrey.tableLength)
		{
			do
				index -= Jeffrey.tableLength;
			while (index >= Jeffrey.tableLength);
		}
		else if (index < 0)
		{
			do
				index += Jeffrey.tableLength;
			while (index < 0);
		}
		// Note: it's OK to round-up index since tables have guard point.
		int ndx = (int) (index + 0.5);
		index += indexIncr;
		compSample = wtPointer[ndx];

			//===================================================================
			//  SCALING THE [-1,1] FLOATS BACK TO 16INT SIZE
			//===================================================================

            /*scaling compSample back up from [-1,1] to 16bit ints as expected 
            by the wav file format, this is needed because sin(x) obviously
            only outputs values from the range [-1,1]*/
			compSample *= Jeffrey.sampleScale;
            
            //bounds-checking the sample
            if(compSample < -Jeffrey.sampleScale){
                compSample = -2^(16-1);
            }
            if( compSample > Jeffrey.sampleScale-1){
                compSample = Jeffrey.sampleScale-1;
            }



            //recasting the scaled float back to the int16
            sample = (Int16) compSample;
            
            //writing the sample to the buffer
            wav.AddSample(sample, n);
          
           
            
            //checking if the phase has overlapped 2PI   
			//only needed now by original sin calculation(none of the class functions)
	    if ((phase += phaseIncr) >= twoPI)
		phase -= twoPI;
            
            
	}

//============================================================================        
//CALL TO WRITE ENTIRE POPULATED BUFFER TO THE FILE WRITER FUNCTION
//============================================================================
        
        //this just calls the wav-writing function to write to a file      
        //wav.WaveFileOut(filename);



//============================================================================
//    WINDOWS API STUFF STARTS HERE
//============================================================================        
        
    /*this implementation is obviously very basic and would be more complex and    
     *class-based, this is just to prove simple funcionality and to make a 
     * window to be used with the audio DirectSound windows stuff since almost
     * all the DirectSound fucntions require a HWND handle of the window
     */    

    

    WNDCLASS wc;
	DEVMODE dmScreenSettings;
	int posX, posY;
    HWND JeffHandle;


	// Give the application a name.
	LPCWSTR applicationName = L"Jeffrey";




	// Setup the windows class with default settings.
	wc.style         = 0;
	wc.lpfnWndProc   = WindowProcedure;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInst;
	wc.hIcon	 = 0;
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = applicationName;
	
	// Register the window class.
	RegisterClass(&wc); 
    

      //(obviously a lot of the windows api stuff should be class-instantiable)
        


        /*creating the window and retrieving the handle to be used in all future
         windows api function calls involving this window, aka handle*/

        JeffHandle = CreateWindow (
        applicationName,            // name of a registered window class
        L"JEFFREY IS ALIIVEEEEEE!",              // window caption
        WS_OVERLAPPEDWINDOW,  // window style
        CW_USEDEFAULT,        // x position
        CW_USEDEFAULT,        // y position
        CW_USEDEFAULT,        // witdh
        CW_USEDEFAULT,        // height
        0,                    // handle to parent window
        0,                    // handle to menu
        hInst,            // application instance
        0);                   // window creation data
        
        //showing the window
        ShowWindow (JeffHandle, cmdShow);
        UpdateWindow (JeffHandle);
        
        MSG  msg;
    int status;
	
    
    OutputDebugString(L"just testing printing lol\n");
	
    
    
//============================================================================        
// DIRECT SOUND PLAYBACK HERE (SEE DirectSoundStuff.h/cpp)
//============================================================================


	   //instantiating class that currently does the directsound stuff
       DSHandler PlayJeff;

	   //creates/sets up DS object and primary buffer
	   PlayJeff.InitializeDirectSound(JeffHandle);

	   //sets up 2ndary buffer and writes to it
	   PlayJeff.InitAndWriteSecBuff(wav.sampleBuffer, wav.numSamples);

	   //plays the secondary buffer
	   PlayJeff.PlaySecBuff();


	   ///needed this fucking sleep() because i played the buffer and killed it right below
	   ///aka the release, so i thought it wasnt playing, but it was, but i just killed it too fast to hear it >:(
	   _sleep((duration*1000) + 500);


	   //releases secondary buffer, primary buffer, and direct sound object
	   PlayJeff.Shutdown();


        

    
  
        
        

        
     /*i think this just waits on the window to be closed? probably need to 
     figure that out tbh, i think ill move this down to after the sound stuff
     so it doesnt prevent my sound code from running*/

        
	  while ((status = GetMessage (& msg, 0, 0, 0)) != 0)
    {
        if (status == -1)
            return -1;
        DispatchMessage (& msg);
    }
    

	return 0;
}
