#include "convert.h"
#include "defines.h"
#include <string.h>

int convertHeader (headerChunk* sample, const char* index){
	unsigned char offset = 0;

	memcpy(sample->header.data, &index[offset], HEADER_BYTES);
	for (int last=HEADER_BYTES-1, first=0; last>(HEADER_BYTES-1)/2; --last, ++first) {
		sample->header.data[first] ^= sample->header.data[last];
		sample->header.data[last]  ^= sample->header.data[first];
		sample->header.data[first] ^= sample->header.data[last];
	}

	memcpy(sample->size.data, &index[offset+HEADER_BYTES], HEADER_BYTES);
	for (int last=HEADER_BYTES-1, first=0; last>(HEADER_BYTES-1)/2; --last, ++first) {
			sample->size.data[first] ^= sample->size.data[last];
			sample->size.data[last]  ^= sample->size.data[first];
			sample->size.data[first] ^= sample->size.data[last];
		}

	memcpy(sample->field.data, &index[offset+2*HEADER_BYTES], HEADER_SIZE);
	for (int last=HEADER_SIZE-1, first=0; last>(HEADER_SIZE-1)/2; --last, ++first) {
			sample->field.data[first] ^= sample->field.data[last];
			sample->field.data[last]  ^= sample->field.data[first];
			sample->field.data[first] ^= sample->field.data[last];
		}

	return (2*HEADER_SIZE+HEADER_BYTES);
}

int convertTrack (trackChunk* sample, const char* index){
	unsigned char offset = 0;

	memcpy(sample->chunk.data, &index[offset], TRACK_BYTES);
	for (int last=TRACK_BYTES-1, first=0; last>(TRACK_BYTES-1)/2; --last, ++first) {
		sample->chunk.data[first] ^= sample->chunk.data[last];
		sample->chunk.data[last]  ^= sample->chunk.data[first];
		sample->chunk.data[first] ^= sample->chunk.data[last];
	}

	memcpy(sample->size.data, &index[offset+TRACK_BYTES], TRACK_BYTES);
	for (int last=TRACK_BYTES-1, first=0; last>(TRACK_BYTES-1)/2; --last, ++first) {
			sample->size.data[first] ^= sample->size.data[last];
			sample->size.data[last]  ^= sample->size.data[first];
			sample->size.data[first] ^= sample->size.data[last];
	}

	return (sample->size.size);
}
