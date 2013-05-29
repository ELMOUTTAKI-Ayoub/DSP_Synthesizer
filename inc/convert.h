#ifndef CONVERT_H_
#define CONVERT_H_

#include "definesMidi.h"

int convertHeader (headerChunk* sample, const char* index);

int convertTrack (trackChunk* sample, const char* index);

int convertMidi(int argc, char **argv, headerChunk* headerSample, trackChunk* trackSample);

#endif /* CONVERT_H_ */
