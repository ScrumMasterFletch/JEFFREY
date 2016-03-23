/*
MIDI message: binary format
 - usually 3 bytes in size
 - information in 7 bits (0-127)
 - 1st byte is the status byte (MSB = 1)
 - 2nd btye is data byte 1 (MSB = 0)
 - 3rd byte is data byte 2 which is optional (MSB = 0)

 -Status byte carries the actual instruction, and channel
   ex:10010100 => 1001       0100
                  note on    channel 5 (decimal # is 4, but 0000 is channel 1, so the channel is this +1)

 -data bytes carry the info about the instruction such as key, velocity(how hard it was pressed), etc


 MESSAGE TYPES: 2 types. channel or system

 Channel messages:
 



*/