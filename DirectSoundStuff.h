/* 
 * File:   DirectSoundStuff.h
 * Author: Austin
 *
 * Created on January 17, 2016, 3:22 PM

 * Very simple playback class, needs to be fleshed out obviously 
 * but this is a good starting point. Needs more in-depth buffer
 * management, position, and write calls. also could add an option
 * to read wavs and play them
 */

#pragma once

#ifndef DIRECTSOUNDSTUFF_H
#define	DIRECTSOUNDSTUFF_H

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")


//////////////
// INCLUDES //
//////////////
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>

class DSHandler
{
public:
	DSHandler();

	~DSHandler();

	//bool Initialize(HWND jHandle); //does entire stuff all the way through play()
	void Shutdown(); //kills 2ndary buff and calls the major shutdown function

//private:
	bool InitializeDirectSound(HWND jHandle); //DS-obj and prim buff get initialized
	void ShutdownDirectSound();  //kills prim buff then DS-obj

	///needing the to be passed number of samples needs to be addressed, and the memcpy() method
	///of writing to the buffer is silly (and circular buffer stuff needs to be implemented)
	bool InitAndWriteSecBuff(Int16 * inBuffer, Uint32 numSamples);
	void ShutdownSecBuff(IDirectSoundBuffer8** secondaryBuffer);

	//bool LoadWaveFile(char*, IDirectSoundBuffer8**); //main work done here
	//void ShutdownWaveFile(IDirectSoundBuffer8**);  //releases 2ndary buffer


	bool PlaySecBuff(); //stes curr volume, position, and then plays

private:
	//Declaring the DSobject
    IDirectSound8* DS_Object;
    //Declaring the primary buffer that handles 2ndary buffers for us
	IDirectSoundBuffer* DS_PrimBuff;   


    //2ndary buffer is what we actually write to
	IDirectSoundBuffer8* DS_SecBuff1;
	///could add more secbuffs if needed, would just need to add
	///simple methods to call the SecBuff.. function with each one

};

#endif	/* DIRECTSOUNDSTUFF_H */
