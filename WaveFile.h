/* 
 * File:   WaveFile.h
 * Author: Austin
 *
 * Created on January 2, 2016, 4:07 PM
 */

#ifndef WAVEFILE_H
#define	WAVEFILE_H
#include "Globals.h"
#include <stdio.h>

using namespace std;
/*gonna need some type of buffer functions: allocation, deallocation, a flush
 *for when the buffer is full(to disk if writing to a file, to the DAC aka the
 * "digital to audio converter" if playing live sounds) etc. 
 * See Wavefile.h/cpp from BasicSynth to get an idea on how this stuff 
 * should work. Also need a function that returns a pointer to the buffer. 
 */


/*THIS STRUCT IS DEFINES EVERYTHING TO BE OUTPUT TO THE WAVE FILE EXCEP THE 
 * ACTUAL SAMPLES AKA THE ACTUAL SOUND DATA.
 * 
 THE CLASS WILL ALSO INCLUDE ITS OWN BUFFER SO IT CAN BE WRITTEN TO 
 INCREMENTALLY WHILE GENERATING THE SAMPLES, INSTEAD OF POPULATIONG AN ENTIRE
 BUFFER OF SAMPLES AND THEN PASSING IT TO THE CLASS*/
//defining the header parts of the wave file, data will be added after
struct WavHeader{
     Uint8 riffId[4];
     Uint32 riffSize;
     Uint8 waveType[4];
     Uint8 fmtId[4];
     Uint32 fmtSize;
     Uint16 fmtCode;
     Uint16 channels;
     Uint32 sampleRate;
     Uint32 avgbps;
     Uint16 align;
     Uint16 bits;
     Uint8 waveId[4];
     Uint32 waveSize;
};

class WaveFile {
public:
    WaveFile();
    WaveFile(Uint16 channels);
    WaveFile(const WaveFile& orig);
    virtual ~WaveFile();
    
    
    
    
    //setters/getters for header values? maybe channels?
    
    /*need to be able to accept # of samples in constructor, pre-calculate 
     * before instantiating? Thats not possible in the case of a user wanting to 
     * press a note of a various length and it get written to a wav. maybe 
     * declare the instantiated sample buffer for the class as dynamic?
     * aka Int16* sampleBuffer;? Maybe just declare a buffer large enough for 
     * like a capped-length sound (44100 X 10 = 441000, 220500 if halfing sample
     * rate) and just keep track of how many indexes of that buffer we fill?
     * side note can the duration variable be a float aka not full seconds?
     * */
    
    //sample buffer of class (max length of 10 seconds)
    //for dynamic playback you could just loop through this buffer again?
    //keep writing this buffer to the directSound one until user releases key?
    //does this need to be 10 seconds then? RIP design choices RIP
    //maybe have 2 different classes for writing to wave files and imm. playback
    Int16 sampleBuffer[441000];
    WavHeader header;
    int numSamples;
    void AddSample(Int16 sample, int n);
    void WaveFileOut(char filename[]);
    
private:

};

#endif	/* WAVEFILE_H */
