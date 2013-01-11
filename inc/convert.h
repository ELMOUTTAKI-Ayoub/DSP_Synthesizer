#ifndef CONVERT_H_
#define CONVERT_H_

#include "defines.h"

int convertHeader (headerChunk* sample, const char* index);

int convertTrack (trackChunk* sample, const char* index);


#endif /* CONVERT_H_ */
