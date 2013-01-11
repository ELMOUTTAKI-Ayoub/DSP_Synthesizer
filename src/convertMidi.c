#include "defines.h"
#include "convert.h"

int main(int argc, char **argv) {

unsigned char verbose = 0;
FILE* pFile;
long lSize;
char* buffer;
size_t result;

// Setzen des Verbosemode's
if (argc>1 && argv[1][1]=='v') {
	verbose++;
	printf("Verbose aktiviert!\n");
}

if ((pFile = fopen(MIDI_FILE, "rb")) == NULL) {
	fprintf(stderr, "\nDas Lesen der MIDI-Datei ist fehlgeschlagen");
	exit(1);
}

// Dateigröße ermitteln
fseek (pFile , 0 , SEEK_END);
lSize = ftell (pFile);
rewind (pFile);

// Speicher alloziieren
if ((buffer = (char*) malloc (sizeof(char)*lSize)) == NULL) {
	fprintf(stderr, "\nDie Speicheralloziierung ist fehlgeschlagen");
	exit(2);
}

// Datei in den Speicher kopieren
if ((result = fread (buffer,1,lSize,pFile)) != lSize) {
	fprintf(stderr, "\nDer kopiervorgang ist fehlgeschlagen");
	exit(2);
}

printf ("Ausgabe der Datei %s\n", MIDI_FILE);
for (int i=0; i < lSize; ++i) {
	static unsigned char trkNmbr = 0;

	if (memcmp(&buffer[i],HEADER_CHUNK,HEADER_BYTES) == 0) {
		struct headerChunk sample;
		i += convertHeader(&sample, &buffer[i]);

		printf("HEADER:\tKennung:%X Size:%i enthält %s, %i Tracks und %i Delta Time-ticks pro Viertelnote",sample.header.header,sample.size.size,HEADER_MSG[sample.field.field[2]],sample.field.field[1],sample.field.field[0]);
	}
	if (memcmp(&buffer[i],TRACK_CHUNK,TRACK_BYTES) == 0) {
		struct trackChunk sample;
		i += convertTrack(&sample, &buffer[i]);

		printf("\nTRACK:\tNummer %u gefunden, mit einer Länge von %i-Bytes", trkNmbr++, sample.size.size);
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
