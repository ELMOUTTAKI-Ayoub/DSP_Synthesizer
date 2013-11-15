#ifndef DEFINES_H_
#define DEFINES_H_
/*
 * Includes um Funktionalitäten des DSPs zu gewährleisten
 */
//#include "dsk6713.h"
//#include "dsk6713_aic23.h"  			//codec, etc.
//#include "dsk6713config.h"

/*
 * C-Bibliotheken
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * Hardware defines
 */
#define DSK6713_AIC23_INPUT_MIC 	0x0015
#define DSK6713_AIC23_INPUT_LINE 	0x0011
//#define SAMPLE_RATE 				DSK6713_AIC23_FREQ_8KHZ
/*
 * Software defines
 */
/* Midi Statusbytes */
#define	NOTE_OFF					0x0008
#define	NOTE_ON						0x0009
#define POLYPHONIC_PRESSURE			0x000A
#define	CONTROL_CHANGE				0x000B
#define	PROGRAMM_CHANGE				0x000C
#define CHANNEL_PRESSURE			0x000D
#define PITCH_BENDING				0x000E
#define SYSTEM_EXCLUSIVE			0x000F
/* Midi Header und Track Chunk */
static const char HEADER_CHUNK[] =  {0x4D, 0x54, 0x68, 0x64};			//ASCII: "MThd"
#define HEADER_BYTES				0x0004
#define HEADER_SIZE					0x0006
static const char TRACK_CHUNK[] = 	{0x4D, 0x54, 0x72, 0x6B};			//ASCII: "MTrk"
#define TRACK_BYTES					0x0004
#define COMMAND_BYTES				0x0002
/* Oktaven Offset */
#define NEXT_OCTAVE					0x000C
/* MidiDatei */
#define MIDI_FILE					"Tracks/LoopDemo.mid"
/* Textausgaben */
static const char* HEADER_MSG[] = 	{"eine Spur","mehrere synchrone Spuren","mehrere asynchrone Spuren"};

/**************************************************************************************************/

/* Definitionen komplexer Datentypen */
typedef union {						// Header Daten des HEADERS oder TRACKs
	unsigned char		data[HEADER_BYTES];
	int32_t 			header;
} header;

typedef union {						// repraesentiert einen Chunk, HEADER oder TRACK
	unsigned char		data[TRACK_BYTES];
	int32_t 			chunk;
} chunk;

typedef union {
	unsigned char		data[HEADER_BYTES];
	int32_t 			size;
} size;

typedef union {						// dieses Datenfeld existiert nur im Header
	unsigned char		data[HEADER_SIZE];
	int16_t				field[HEADER_SIZE/2];
} field;

typedef struct {							// das Kommando besteht aus zwei Bytes
	uint16_t				command;// welche Nibble-Weise Informationen enthalten
	uint16_t				channel;
	uint16_t				deltaTime;
	uint16_t				noteNumber;
	uint16_t				noteVelocity;
} command;

typedef struct {
	header 				header;
	size				size;
	field 				field;
} headerChunk;

typedef struct {
	chunk 				chunk;
	size				size;
	uint16_t 			counter;			// Zähler für Sinuserzäugung
	uint16_t			delayCounter;		// Zähler bis zum nächsten Ereignis
	int16_t			commandCounter;		// Platzhalter für das aktuell ausführende Event; -1 kein Commando
} trackChunk;

typedef struct {				// Typedef welches die Tracks der weiteren
	int8_t 				numTracks;		// Verarbeitung zur Verfuegung stellt
	trackChunk* 		tracks;
	command**			commands;
} midiTracks;

extern midiTracks globalTracks;

#endif /* DEFINES_H_ */
