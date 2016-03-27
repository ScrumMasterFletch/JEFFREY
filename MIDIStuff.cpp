#include "MIDIStuff.h"


//constructor
MIDIReceiver::MIDIReceiver(){
    mNumKeys = 0;
    mOffset = 0;

    for (int i = 0; i < keyCount; i++) {
         mKeyStatus[i] = false;
	}

}


///this function is called on the receival of every MIDI message
//is called in ProcessMidiMsg, a IPlug class to handle MIDI that we overwrite in MyFirstPlugin
void MIDIReceiver::onMessageReceived(IMidiMsg* midiMessage) {


	///this gets the StatusMessage enum from the MIDImessage struct(its pretty much a class, it has functions)
	//StatusMsg() right-shifts the 8-bit EStatusMsg by 4, so the number returned is the first 4 bits of the status byte
	//which contains the actual instruction, the bits 5-8 designate the channel
    IMidiMsg::EStatusMsg status = midiMessage->StatusMsg();


	///this checks if the first 4 bits of the message are either 1000 or 1001, if so, adds it to queue
    //1000 is note off, 1001 is note-on, decimal values being 8 and 9 respectively
	// We're only interested in Note On/Off messages (not CC, pitch, etc.)
    if(status == IMidiMsg::kNoteOn || status == IMidiMsg::kNoteOff) {
        mMidiQueue.Add(midiMessage);
    }
}



void MIDIReceiver::advance() {
    while (!mMidiQueue.Empty()) {
		//Peek() grabs the next message (front of queue) without actually removing it from the queue
		IMidiMsg* midiMessage = mMidiQueue.Peek();

        if (midiMessage->mOffset > mOffset) break;

		//i described what StatusMsg() does in the above call to it in OnMessageReceived
        IMidiMsg::EStatusMsg status = midiMessage->StatusMsg();

		//NoteNumber returns the data1 byte from the MIDI message aka the key/note number
        int noteNumber = midiMessage->NoteNumber();

		//Velocity() returns the data2 byte from the MIDI message aka the velocity
        int velocity = midiMessage->Velocity();


        /// There are only note on/off messages in the queue, see ::OnMessageReceived
		//if message was note on and velocity >0, update the keyStatus array for that note to true
        if (status == IMidiMsg::kNoteOn && velocity) 
		{
            if(mKeyStatus[noteNumber] == false) 
			{
                mKeyStatus[noteNumber] = true;
                mNumKeys += 1;
				noteOn(noteNumber, velocity);
            }

        } 
		else //either it was a note-off msg or the velocity is 0 (both mean sound should stop)
		{ 
			//takes away note no longer playing
            if(mKeyStatus[noteNumber] == true) {
                mKeyStatus[noteNumber] = false;
                mNumKeys -= 1;
				noteOff(noteNumber, velocity);
            }
        }//else

        mMidiQueue.Remove(); //now actually removes the Peek()ed message
    }
    mOffset++; //adjusts the offset
}