/*
 * notes.h
 *
 *  Created on: 26.03.2013
 *      Author: Matthi Laptop
 */

#ifndef NOTES_H_
#define NOTES_H_

#include "stdint.h"

/*
 * Software defines
 */
#define MUXH 	0x019C0000
#define PI 		3.14159265359f
#define VOLUME	1				//max. Lautstaerke
#define QUARTER_NOTE_TIME 0.5   //Laenge einer 1/4 Note in Sekunden
#define SAMPLE_RATE 44000


extern float volume_DSP;		//output Volume

extern short sinus[SAMPLE_RATE]; 		//lockup table

typedef struct {
	uint16_t 	freq;
	uint8_t		canal;
	uint8_t 	noteOn;
	uint16_t 	deltaTime;
	uint16_t 	counter;
}note;


int create_note (short num);

void create_lut_sinus(void);

void volume_up(void);

void volume_down(void);

void fill_toene(note* track, uint8_t trackcount);

#endif /* NOTES_H_ */
