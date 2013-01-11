#ifndef CONVERT_H_
#define CONVERT_H_

#include "defines.h"

int convertHeader (struct headerChunk* sample, const char* index);

int convertTrack (struct trackChunk* sample, const char* index);


#endif /* CONVERT_H_ */
