/* 
 * File:   WaveFile.cpp
 * Author: Austin
 * 
 * Created on January 2, 2016, 4:07 PM
 */



#include "WaveFile.h"
#include "Globals.h"


//constructor 
WaveFile::WaveFile() {
    
    //====================================================================
    // POPULATING THE HEADER STRUCT
    //====================================================================
    header.riffId[0] = 'R';
    header.riffId[1] = 'I';
    header.riffId[2] = 'F';
    header.riffId[3] = 'F';
    
    //this will need to be calculated right before writing when we have data siz
    //header.riffSize = byteTotal + sizeof(header) - 8;
    header.riffSize = 0;
    
    header.waveType[0] = 'W';
    header.waveType[1] = 'A';
    header.waveType[2] = 'V';
    header.waveType[3] = 'E';
    header.fmtId[0] = 'f';
    header.fmtId[1] = 'm';
    header.fmtId[2] = 't';
    header.fmtId[3] = ' ';
    header.fmtSize = 16;
    header.fmtCode = 1;
    
    header.channels = 1;
    //header.channels = (Uint16) channels;
    
    header.sampleRate = 44100;
    header.bits = 16;
    header.align = (header.channels * header.bits) /8;
    header.avgbps = (header.sampleRate * header.align);
    header.waveId[0] = 'd';
    header.waveId[1] = 'a';
    header.waveId[2] = 't';
    header.waveId[3] = 'a';
    
    //this will need to be calculated right before writing when we have data siz
    //header.waveSize = byteTotal;
    header.waveSize = 0;
    
    numSamples = 0;
}



//constructor that takes in # of channels
WaveFile::WaveFile(Uint16 channels) {
    
    //====================================================================
    // POPULATING THE HEADER STRUCT
    //====================================================================
    header.riffId[0] = 'R';
    header.riffId[1] = 'I';
    header.riffId[2] = 'F';
    header.riffId[3] = 'F';
    
    //this will need to be calculated right before writing when we have data siz
    //header.riffSize = byteTotal + sizeof(thing) - 8;
    header.riffSize = 0;
    
    header.waveType[0] = 'W';
    header.waveType[1] = 'A';
    header.waveType[2] = 'V';
    header.waveType[3] = 'E';
    header.fmtId[0] = 'f';
    header.fmtId[1] = 'm';
    header.fmtId[2] = 't';
    header.fmtId[3] = ' ';
    header.fmtSize = 16;
    header.fmtCode = 1;
    header.channels = channels;
    header.sampleRate = 44100;
    header.bits = 16;
    header.align = (header.channels * header.bits) /8;
    header.avgbps = (header.sampleRate * header.align);
    header.waveId[0] = 'd';
    header.waveId[1] = 'a';
    header.waveId[2] = 't';
    header.waveId[3] = 'a';
    
    //this will need to be calculated right before writing when we have data siz
    //header.waveSize = byteTotal;
    header.waveSize = 0;
    
    numSamples = 0;
}



WaveFile::WaveFile(const WaveFile& orig) {
}

WaveFile::~WaveFile() {
}

//function that actually writes the buffer to a file
void WaveFile::WaveFileOut(char* filename){
    
    Uint32 byteTotal = (Uint32) numSamples * 2 * header.channels;
    
    header.waveSize = byteTotal;
    
    header.riffSize = (Uint32) byteTotal + sizeof(header) - 8;
    
    
    //file-write stuff here
    FILE *f = fopen(filename, "wb");
    fwrite(&header, 1, sizeof(header), f);
    fwrite(sampleBuffer, 2*header.channels, numSamples, f);
    fclose(f);
}

void WaveFile::AddSample(Int16 sample, int n){
    sampleBuffer[n] = sample;
    numSamples++;
}
