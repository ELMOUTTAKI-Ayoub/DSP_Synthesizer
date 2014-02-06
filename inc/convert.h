#ifndef CONVERT_H_
#define CONVERT_H_

#include "definesMidi.h"

int convertHeader (headerChunk* sample, const uint8_t* index);

int convertEvent (trackChunk* sample, const uint8_t* index, uint32_t* deltaTime, uint32_t* events);

int convertTrack (trackChunk* sample, const uint8_t* index);


void printTrackChunk(trackChunk chunk,int8_t trackNumber);
void printCommadBegin(uint8_t trackNumber);
void printCommand(command com);
void printBeginCommand();
void printEndCommand();

void printCommandList(uint8_t commandCount);

void printChunkList(midiTracks* tracks);
void printMidi(uint8_t trackCount);

int convertMidi(int argc, char **argv, headerChunk* headerSample);


#endif /* CONVERT_H_ */
