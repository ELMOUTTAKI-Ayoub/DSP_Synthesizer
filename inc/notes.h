/*
 * notes.h
 *
 *  Created on: 26.03.2013
 *      Author: Matthi Laptop
 */

#ifndef NOTES_H_
#define NOTES_H_

#include "stdint.h"
#include "definesMidi.h"

/**
 * Software defines
 */
#define MUXH 				0x019C0000		// Addresse des MUXH
#define PI 					3.14159265359f	// Zahl PI
#define VOLUME				1				// max. Lautstaerke
#define QUARTER_NOTE_TIME 	0.5   			// Laenge einer 1/4 Note in Sekunden
#define SAMPLE_RATE 		44000			// Samplerate

/**
 * deklaration der externen Variabelen
 */
extern float volume_DSP;			//Lautstärke der DSP Boards
extern short sinusLUT[SAMPLE_RATE];	//lockuptabelle

/**
 * Funktion zur Umrechnung aller DeltaTime eines MIDI in eine Samplerate
 * @param midiSample
 */
void convert_delta_time (midiTracks* midiSample);

/**
 * Funktion zur Umrechnung aller Notennummern eines MIDI in Frequenzen
 * @param midiSample
 */
void convert_note_number (midiTracks* midiSample);

/**
 * Wandelt Ausgelesene Notenummer in eine Frequenz um.
 * @param num = Notenwert
 * @return (int) Frequenzwert
 */
int create_note (short num);

/**
 * Füllt Lookup Tabelle mit dem Sinus-Signal
 */
void create_lut_sinus(void);

/**
 * TODO: Kann wieder gelöscht werden.
 * @param midiSample
 */
void fill_toene(midiTracks* midiSample);

#endif /* NOTES_H_ */
