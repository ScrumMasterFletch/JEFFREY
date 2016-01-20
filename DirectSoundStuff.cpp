

#include "Globals.h"
#include "DirectSoundStuff.h"

//Constructor. All this does is initialize the pointers to 0
DSHandler::DSHandler(){
	DS_Object = 0;
    DS_PrimBuff = 0;   
    DS_SecBuff1 = 0;
}


DSHandler::~DSHandler(){
}

///is not needed as of now, combines call to this class into 1
//does entire stuff all the way through play()
/*bool DSHandler::Initialize(HWND jHandle){
	bool result;


	// Initialize direct sound and the primary sound buffer.
	result = InitializeDirectSound(jHandle);
	if(!result)
	{
		return false;
	}

	// Load a wave audio file onto a secondary buffer.
	result = ("../Engine/data/sound01.wav", &m_secondaryBuffer1);
	if(!result)
	{
		return false;
	}

	// Play the wave file now that it has been loaded.
	result = PlayWaveFile();
	if(!result)
	{
		return false;
	}

	return true;
} */





void DSHandler::Shutdown(){
	// Release the secondary buffer.
	ShutdownSecBuff(&DS_SecBuff1);

	// Shutdown the Direct Sound API.
	ShutdownDirectSound();


} //kills 2ndary buff and calls the major shutdown function







bool DSHandler::InitializeDirectSound(HWND jHandle){
	
	// Initialize direct sound and the primary sound buffer.
	HRESULT result2;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;

	///CREATING THE DS_OBJECT
	// Initialize the direct sound interface pointer for the default sound device.
	result2 = DirectSoundCreate8(NULL, &DS_Object, NULL);
	if(FAILED(result2))
	{
            OutputDebugString(L"failed to create direct sound object");
		//return false;
	}



	// Set the cooperative level to priority so the format of the primary sound buffer can be modified.
	result2 = DS_Object->SetCooperativeLevel(jHandle, DSSCL_PRIORITY);
	if(FAILED(result2))
	{
            OutputDebugString(L"failed to set priority");
		//return false;
	}



	///PRIMARY BUFFER STUFF HERE//////////////////////////////////////////////////////
	// Setup the primary buffer description.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Get control of the primary sound buffer on the default sound device.
	result2 = DS_Object->CreateSoundBuffer(&bufferDesc, &DS_PrimBuff, NULL);
	if(FAILED(result2))
	{
            OutputDebugString(L"failed to take control of prim buffer");
		//return false;
	}

	// Formatting the primary sound buffer.
	// In this case it is a .WAV file recorded at 44,100 samples per second in 16-bit stereo (cd audio format).
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100; //NEEDS TO BE Jeffrey.sampleRate;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 1;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;


	// Set the primary buffer to be the wave format specified.
	result2 = DS_PrimBuff->SetFormat(&waveFormat);

	return true;
}






void DSHandler::ShutdownDirectSound(){

	if(DS_PrimBuff)
	{
		DS_PrimBuff->Release();
		DS_PrimBuff = 0;
	}

	// Release the direct sound interface pointer.
	if(DS_Object)
	{
		DS_Object->Release();
		DS_Object = 0;
	}

}//kills prim buff then DS-obj






bool DSHandler::InitAndWriteSecBuff(Int16 * inBuffer, Uint32 numSamples){

	//initializing tempBuff to be used to setup the secondary buff
	HRESULT result2;
    DSBUFFERDESC bufferDesc2;
    IDirectSoundBuffer* tempBuffer;
    WAVEFORMATEX waveFormat2;    

    //These are used in the Lock() call involving the secondary buffer
    unsigned char* bufferPtr;
	unsigned long bufferSize;


	// Formatting the secondary sound buffer.
	waveFormat2.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat2.nSamplesPerSec = 44100; //NEEDS TO BE Jeffrey.sampleRate?
	waveFormat2.wBitsPerSample = 16;
	waveFormat2.nChannels = 1;
	waveFormat2.nBlockAlign = (waveFormat2.wBitsPerSample / 8) * waveFormat2.nChannels;
	waveFormat2.nAvgBytesPerSec = waveFormat2.nSamplesPerSec * waveFormat2.nBlockAlign;
	waveFormat2.cbSize = 0;


	// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
	bufferDesc2.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc2.dwFlags = DSBCAPS_CTRLVOLUME;
    // *2 is for 2 bytes per sample(16 bits),

	DWORD size = (DWORD) (numSamples * 2 * waveFormat2.nChannels);

	bufferDesc2.dwBufferBytes = size;
	bufferDesc2.dwReserved = 0;
	bufferDesc2.lpwfxFormat = &waveFormat2;
	bufferDesc2.guid3DAlgorithm = GUID_NULL;
        

    // Create a temporary sound buffer with the specific buffer settings.
	result2 = DS_Object->CreateSoundBuffer(&bufferDesc2, &tempBuffer, NULL);
	if(FAILED(result2))
	{
            OutputDebugString(L"failed to create tempBuff");
		//return false;
	}



	//DID THIS TO MAKE SENSE OF EXAMPLE THAT USED CLASS FUCNTION CALLS AND PASSING POINTERS
	///need to come through and clear up this secondaryBuffer to DS_secBuff1 stuff 
	///(its left over from hardcoding-to-functions conversion)
	IDirectSoundBuffer8** secondaryBuffer = &DS_SecBuff1;


	// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	result2 = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if(FAILED(result2))
	{
            OutputDebugString(L"failed to set up secondary buffer");
		//return false;
	}
    


	// Release the temporary buffer.
	//This is done because the queryInterface call created the secondary buffer
	tempBuffer->Release();
	tempBuffer = 0;


	// Lock the secondary buffer to write wave data into it.
	result2 = (*secondaryBuffer) ->Lock(0, size, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if(FAILED(result2))
	{
		OutputDebugString(L"failed to lock secondary buffer");
		//return false;
	}
        

	// Copy the wave data into the buffer.
	memcpy(bufferPtr, inBuffer, size);

	

	// Unlock the secondary buffer after the data has been written to it.
	result2 = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if(FAILED(result2))
	{
		OutputDebugString(L"failed to unlock secondary buffer");
		//return false;
	}

	return true;
}



void DSHandler::ShutdownSecBuff(IDirectSoundBuffer8** secondaryBuffer){

	if(*secondaryBuffer)
	{
		(*secondaryBuffer)->Release();
		*secondaryBuffer = 0;
	}

}



bool DSHandler::PlaySecBuff(){

	HRESULT result2;

	// Set position at the beginning of the sound buffer.
	result2 = DS_SecBuff1->SetCurrentPosition(0);
	if(FAILED(result2))
	{
		OutputDebugString(L"failed to set current position to play");
		//return false;
	}


	// Set volume of the buffer to 100%.
	result2 = DS_SecBuff1->SetVolume(DSBVOLUME_MAX);
	if(FAILED(result2))
	{
		OutputDebugString(L"failed to set volume to max");
		//return false;
	}


	// Play the contents of the secondary sound buffer.
	result2 = DS_SecBuff1->Play(0, 0, 0);
	if(FAILED(result2))
	{
		OutputDebugString(L"failed to play :(");
		//return false;
	}

	return true;
}//stes curr volume, position, and then plays
