#include "convert.h"

#include <string.h>

int last, first;

int convertHeader (headerChunk* sample, const char* index){
	unsigned char offset = 0;

	memcpy(sample->header.data, &index[offset], HEADER_BYTES);
	for (last=HEADER_BYTES-1, first=0; last>(HEADER_BYTES-1)/2; --last, ++first) {
		sample->header.data[first] ^= sample->header.data[last];
		sample->header.data[last]  ^= sample->header.data[first];
		sample->header.data[first] ^= sample->header.data[last];
	}

	memcpy(sample->size.data, &index[offset+HEADER_BYTES], HEADER_BYTES);
	for (last=HEADER_BYTES-1, first=0; last>(HEADER_BYTES-1)/2; --last, ++first) {
			sample->size.data[first] ^= sample->size.data[last];
			sample->size.data[last]  ^= sample->size.data[first];
			sample->size.data[first] ^= sample->size.data[last];
		}

	memcpy(sample->field.data, &index[offset+2*HEADER_BYTES], HEADER_SIZE);
	for (last=HEADER_SIZE-1, first=0; last>(HEADER_SIZE-1)/2; --last, ++first) {
			sample->field.data[first] ^= sample->field.data[last];
			sample->field.data[last]  ^= sample->field.data[first];
			sample->field.data[first] ^= sample->field.data[last];
		}

	return (2*HEADER_SIZE+HEADER_BYTES);
}

int convertTrack (trackChunk* sample, const char* index){
	unsigned char offset = 0;

	memcpy(sample->chunk.data, &index[offset], TRACK_BYTES);
	for (last=TRACK_BYTES-1, first=0; last>(TRACK_BYTES-1)/2; --last, ++first) {
		sample->chunk.data[first] ^= sample->chunk.data[last];
		sample->chunk.data[last]  ^= sample->chunk.data[first];
		sample->chunk.data[first] ^= sample->chunk.data[last];
	}

	memcpy(sample->size.data, &index[offset+TRACK_BYTES], TRACK_BYTES);
	for (last=TRACK_BYTES-1, first=0; last>(TRACK_BYTES-1)/2; --last, ++first) {
			sample->size.data[first] ^= sample->size.data[last];
			sample->size.data[last]  ^= sample->size.data[first];
			sample->size.data[first] ^= sample->size.data[last];
	}

	return (sample->size.size);
}

//int convertMidi (int argc, char **argv, midiTracks* sample, const char* index){
//	unsigned char offset = 0;
//
//	return (sample->tracks);
//}

int convertMidi(int argc, char **argv, headerChunk* headerSample, trackChunk* trackSample) {

	unsigned char verbose = 0;
	FILE* pFile;
	long lSize;
	char* buffer;
	size_t result;
	int i;

	/*
	 * Wünscht der Nutzer die Ausgabe der kompletten Datei ASCII-kodiert,
	 * so hat er den Verbose Mode mit dem Parameter -v aktiviert.
	 */
	if (argc>1 && argv[1][1]=='v') {
		verbose++;
		printf("Verbose aktiviert!\n");
	}

	if ((pFile = fopen(MIDI_FILE, "rb")) == 0) {
		fprintf(stderr, "\nDas Lesen der MIDI-Datei ist fehlgeschlagen");
		exit(1);
	}

	/*
	 * Der Dateizeiger wird mit dem Aufruf von fseek() ans Ende der Datei gesetzt
	 * und sollte somit fï¿½r das spï¿½tere einlesen wieder an den Anfang gesetzt werden.
	 */
	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);

	// Speicher alloziieren
	if ((buffer = (char*) malloc (sizeof(char)*lSize)) == 0) {
		fprintf(stderr, "\nDie Speicheralloziierung ist fehlgeschlagen");
		exit(2);
	}

	// Datei in den Speicher kopieren
	if ((result = fread (buffer,1,lSize,pFile)) != lSize) {
		fprintf(stderr, "\nDer Kopiervorgang ist fehlgeschlagen");
		exit(2);
	}

	printf ("Ausgabe der Datei %s", MIDI_FILE);
	for (i=0; i < lSize; ++i) {
		static unsigned char trkNmbr = 1;

		if (memcmp(&buffer[i],HEADER_CHUNK,HEADER_BYTES) == 0) {
			//headerChunk sample;
			i += convertHeader(headerSample, &buffer[i]);

			printf("\nHEADER:\tKennung:%X Size:%i enthaelt %s, %i Tracks und %i Delta Time-ticks pro Viertelnote",headerSample->header.header,headerSample->size.size,HEADER_MSG[headerSample->field.field[2]],headerSample->field.field[1],headerSample->field.field[0]);
		}
		else if (memcmp(&buffer[i],TRACK_CHUNK,TRACK_BYTES) == 0) {
			//trackChunk sample;
			i += convertTrack(trackSample, &buffer[i]);

			printf("\nTRACK:\tNummer %u gefunden, mit einer Laenge von %i-Bytes", trkNmbr++, trackSample->size.size);
		}
		if (verbose == 1) {
			buffer[i]=='\n' ? printf("\n") : printf("%X", (unsigned int)buffer[i]);
		}
	}

	printf("\n");
	fclose(pFile);
	free(buffer);
	return 0;
}
