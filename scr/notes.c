/*
 *  notes.c
 *
 *  Created on: 13.03.2013
 *      Author: Matthis Bukovsky
 */
#include "notes.h"
#include "math.h"

union toene;

//note test_midi[] = {event1, event2, event3, event4, event5, event6, event7, event8  };

/**
 * Wandelt Ausgelesene Notenummer in eine Frequenz um.
 *
 *	Oktave||                     Notennummer
 *	   #  ||
 *		  || C   | C#  | D   | D#  | E   | F   | F#  | G   | G#  | A   | A#  | H
 *	-----------------------------------------------------------------------------
 *	   0  ||   0 |   1 |   2 |   3 |   4 |   5 |   6 |   7 |   8 |   9 |  10 | 11
 *	   1  ||  12 |  13 |  14 |  15 |  16 |  17 |  18 |  19 |  20 |  21 |  22 | 23
 *	   2  ||  24 |  25 |  26 |  27 |  28 |  29 |  30 |  31 |  32 |  33 |  34 | 35
 *	   3  ||  36 |  37 |  38 |  39 |  40 |  41 |  42 |  43 |  44 |  45 |  46 | 47
 *	   4  ||  48 |  49 |  50 |  51 |  52 |  53 |  54 |  55 |  56 |  57 |  58 | 59
 *	   5  ||  60 |  61 |  62 |  63 |  64 |  65 |  66 |  67 |  68 |  69 |  70 | 71
 *	   6  ||  72 |  73 |  74 |  75 |  76 |  77 |  78 |  79 |  80 |  81 |  82 | 83
 *	   7  ||  84 |  85 |  86 |  87 |  88 |  89 |  90 |  91 |  92 |  93 |  94 | 95
 *	   8  ||  96 |  97 |  98 |  99 | 100 | 101 | 102 | 103 | 104 | 105 | 106 | 107
 *	   9  || 108 | 109 | 110 | 111 | 112 | 113 | 114 | 115 | 116 | 117 | 118 | 119
 *	  10  || 120 | 121 | 122 | 123 | 124 | 125 | 126 | 127 |
 *
 * @param num = Notenwert (Siehe Tabelle)
 * @return (int) Frequenzwert
 */
int create_note (short num) {
	float div;
	float c = 8.175798916f;		// Konstante
	/* Berechnung der Frequenz
	 *   f= c * 2^(num/12)
	 */
	return (int)(c * powf(2.0f, div));
}


/**
 * Füllt Lookup Tabelle mit dem Sinus-Signal
 */
void create_lut_sinus(void) {

	double x = 0;
	uint16_t i = 0;

	for (i = 0; i < SAMPLE_RATE; ++i) {
		// sinus sin(2 * pi * f * t)
		sinus[i] = (short)(0X7FFF*sin(x));
		x = 2*PI*i/SAMPLE_RATE;
	}
}

/**
 * Umwandlung aller DeltaTime des Midi in ein atequate Samplerate
 * @param midiSample
 */
void convert_delta_time (midiTracks* midiSample) {

	uint16_t time_dev;
	uint8_t track_index=0;

	// durchläuft jeden Track
	while (track_index < midiSample->numTracks) {

		time_dev = midiSample->tracks[track_index].chunk.chunk;
		uint8_t event_index=0;
		// durchläuft in jedem Track die Events
		while (event_index < midiSample->tracks[track_index].size.size) {

			/**
			 * wandelt die DeltaTime eines Events in eine Samplerate um.
			 * TODO: schiften mit >> 4 bewirkt das max. 16s ein erneutes Event aufgerufen werden muss.
			 */
			midiSample->commands[track_index][event_index].deltaTime = (uint32_t)( (midiSample->commands[track_index][event_index].deltaTime * QUARTER_NOTE_TIME / time_dev) * SAMPLE_RATE ) >> 4;
			event_index++;	// sring zum nächsten Event
		}
		track_index++;	// spring zum nächsten Track
	}
}

/**
 * Umwandlung aller Notennummern des Midi in Frequenzen
 * @param midiSample
 */
void convert_note_number (midiTracks* midiSample) {

	uint8_t track_index=0;
	// durchläuft jeden Track
	while (track_index < midiSample.numTracks) {

		uint8_t event_index=0;
		// durchläuft in jedem Track die Events
		while (event_index < midiSample.tracks[track_index].size.size) {

			// Umwandlung der Notennummer eines Events in eine Frequenz
			midiSample.commands[track_index][event_index].noteNumber = create_note((short)midiSample.commands[track_index][event_index].noteNumber);
			event_index++;	// sring zum nächsten Event
		}
		track_index++;	// spring zum nächsten Track
	}
}

/**
 * fuellen der Trackspuren mit Signalen
 * TODO: muss wieder gelöscht werden.
 * @param midiSample
 */
void fill_toene (midiTracks* midiSample) {

	midiSample->numTracks = 2;

	midiSample->tracks[0].counter= 0;
	midiSample->tracks[0].commandCounter = -1;
	midiSample->tracks[0].delayCounter = 0;
	midiSample->tracks[0].chunk.chunk = 480;
	midiSample->tracks[0].size.size= 200;

	midiSample->tracks[1].counter= 0;
	midiSample->tracks[1].commandCounter = -1;
	midiSample->tracks[1].delayCounter = 0;
	midiSample->tracks[1].chunk.chunk = 240;
	midiSample->tracks[1].size.size= 0;
	uint8_t i;
	for( i=0;i<200;i+=2){
		midiSample->commands[0][i].command = NOTE_ON;
		midiSample->commands[0][i].noteNumber = i/2;
		midiSample->commands[0][i].deltaTime = i;

		midiSample->commands[0][i+1].command = NOTE_OFF;
		midiSample->commands[0][i+1].noteNumber = i/2;
		midiSample->commands[0][i+1].deltaTime = 10*i;
	}

//	midiSample->commands[0][0].command = NOTE_ON;
//	midiSample->commands[0][0].noteNumber = 23;
//	midiSample->commands[0][0].deltaTime = 0;
//
//	midiSample->commands[0][1].command = NOTE_OFF;
//	midiSample->commands[0][1].noteNumber = 23;
//	midiSample->commands[0][1].deltaTime = 480;
//
//	midiSample->commands[0][2].command = NOTE_ON;
//	midiSample->commands[0][2].noteNumber = 44;
//	midiSample->commands[0][2].deltaTime = 240;
//
//	midiSample->commands[0][3].command = NOTE_OFF;
//	midiSample->commands[0][3].noteNumber = 44;
//	midiSample->commands[0][3].deltaTime = 960;

	midiSample->commands[1][0].command = NOTE_ON;
	midiSample->commands[1][0].noteNumber = 69;
	midiSample->commands[1][0].deltaTime = 0;

	midiSample->commands[1][1].command = NOTE_OFF;
	midiSample->commands[1][1].noteNumber = 69;
	midiSample->commands[1][1].deltaTime = 240;

	midiSample->commands[1][2].command = NOTE_ON;
	midiSample->commands[1][2].noteNumber = 55;
	midiSample->commands[1][2].deltaTime = 240;

	midiSample->commands[1][3].command = NOTE_OFF;
	midiSample->commands[1][3].noteNumber = 55;
	midiSample->commands[1][3].deltaTime = 960;

	midiSample->commands[1][4].command = NOTE_ON;
	midiSample->commands[1][4].noteNumber = 90;
	midiSample->commands[1][4].deltaTime = 0;

	midiSample->commands[1][5].command = NOTE_OFF;
	midiSample->commands[1][5].noteNumber = 90;
	midiSample->commands[1][5].deltaTime = 480;

	midiSample->commands[1][6].command = NOTE_ON;
	midiSample->commands[1][6].noteNumber = 120;
	midiSample->commands[1][6].deltaTime = 240;

	midiSample->commands[1][7].command = NOTE_OFF;
	midiSample->commands[1][7].noteNumber = 120;
	midiSample->commands[1][7].deltaTime = 960;

	midiSample->commands[1][8].command = NOTE_ON;
	midiSample->commands[1][8].noteNumber = 37;
	midiSample->commands[1][8].deltaTime = 0;

	midiSample->commands[1][9].command = NOTE_OFF;
	midiSample->commands[1][9].noteNumber = 37;
	midiSample->commands[1][9].deltaTime = 480;

}

