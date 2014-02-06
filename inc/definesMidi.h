#ifndef DEFINES_H_
#define DEFINES_H_
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
#define DSK6713_AIC23_INPUT_MIC   0x0015
#define DSK6713_AIC23_INPUT_LINE 	0x0011
/*
 * Software defines
 */
#define EVENT_MASK                0xF0
/*
 * Midi Statusbytes
 */
#define	NOTE_OFF                  0x80
#define	NOTE_ON                   0x90
#define POLYPHONIC_PRESSURE       0xA0
#define	CONTROL_CHANGE            0xB0
#define	PROGRAMM_CHANGE           0xC0
#define CHANNEL_PRESSURE          0xD0
#define PITCH_BENDING             0xE0
#define SYSTEM_EXCLUSIVE          0xF0
/*
 * Meta Events
 */
#define CHANNEL_SCOPE             0x21
#define TRACK_END                 0x2F
#define TEMPO_CHANGE              0x51
#define SMPTE_OFFSET              0x54
#define TIME_SIGNATURE            0x58
#define KEY_SIGNATURE             0x59
/*
 * Midi Header und Track Chunk
 */
static const char HEADER_CHUNK[] = { 0x4D, 0x54, 0x68, 0x64 };       //ASCII: "MThd"
#define HEADER_BYTES              0x0004
#define HEADER_SIZE               0x0006
static const char TRACK_CHUNK[] = { 0x4D, 0x54, 0x72, 0x6B };       //ASCII: "MTrk"
#define TRACK_BYTES               0x0004
#define COMMAND_BYTES             0x0002
/* Oktaven Offset */
#define NEXT_OCTAVE               0x000C
/* MidiDateien */
#define MIDI_FILE                 "playMe.mid"
#define MIDI_FILE_OUT             "playMe.h"
/* Textausgaben */
static const char* HEADER_MSG[] = { "eine Spur", "mehrere synchrone Spuren", "mehrere asynchrone Spuren" };

/**************************************************************************************************/

/*
 * Definitionen komplexer Datentypen
 */
typedef union
{								          // Header Daten des HEADERS oder TRACKs
  unsigned char data[HEADER_BYTES];
  int32_t header;
} header;

typedef union
{								          // repraesentiert einen Chunk, HEADER oder TRACK
  unsigned char data[TRACK_BYTES];
  int32_t chunk;
} chunk;

typedef union
{
  unsigned char data[HEADER_BYTES];
  int32_t size;
} size;

typedef union
{								          // dieses Datenfeld existiert nur im Header
  unsigned char data[HEADER_SIZE];
  int16_t field[HEADER_SIZE / 2];
} field;

typedef struct
{							            // das Kommando besteht aus zwei Bytes
  uint32_t command        :  4;       // welche Nibble-Weise Informationen enthalten
  uint32_t channel        :  4;
  uint32_t deltaTime      : 16;
  uint32_t noteNumber     : 16;
} command;

typedef struct
{
  header header;
  size size;
  field field;
} headerChunk;

typedef struct
{
  chunk chunk;
  size size;              // Anzahl der Events
  uint16_t counter;       // Zähler für Sinuserzeugung
  uint16_t delayCounter;  // Zähler bis zum nächsten Ereignis
  int16_t commandCounter; // Platzhalter für das aktuell ausführende Event; -1 kein Commando
} trackChunk;

typedef struct
{							            // Typedef welches die Tracks der weiteren
  int8_t numTracks;       // Verarbeitung zur Verfuegung stellt
  trackChunk* tracks;
  command** commands;
  int16_t ppqn;
} midiTracks;

#endif /* DEFINES_H_ */
