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
#define VOLUME	1				//max. Lautstärke
#define QUARTER_NOTE_TIME 0.5   //Länge einer 1/4 Note in Sekunden
#define SAMPLE_RATE 8000


extern float volume_DSP;		//output Volume

extern short sinus[8000]; 		//lockup table

typedef struct {
	uint16_t freq;
	uint8_t noteOn;
	uint8_t noteOff;
	uint8_t deltaTime;
	uint16_t counter;
}note;

//extern union ton {
//	note  note;
//	short time;
//	short onsetTime;  	//Zeit des Anklingen
//	short subsideTime; 	//Zeit des Abklingen
//};

int create_note (short num);

void create_lut_sinus(void);

void volume_up(void);

void volume_down(void);

//void control_time(void);


#endif /* NOTES_H_ */
