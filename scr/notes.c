/*
 *  notes.c
 *
 *  Created on: 13.03.2013
 *      Author: Matthis Bukovsky
 */
#include "notes.h"
#include "math.h"

note s_note;
short sinus[SAMPLE_RATE]; 			//lockup table
//uint8_t i= 0, toggle= 1;
union toene;


/*
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
 */

int create_note (short num) {
	float div;
	float c = 8.175798916f;

	/* Berechnung der Frequenz
	 *
	 *   f= c * 2^(num/12)
	 */
	div=(float)(num)/12.0f;
	s_note.freq =c * powf(2.0f, div);
	return (int)(c * powf(2.0f, div));
	//tmp.octave = (short)(num/12);
}


/*
 * fill the Lookup Table with the Sinus-signal
 */
void create_lut_sinus(void) {

	double x = 0;
	uint16_t i = 0;

	for (i = 0; i < SAMPLE_RATE; ++i) {
		sinus[i] = (short)(0X7FFF*sin(x));
		// sinus sin(2 * pi * f * t)
		x = 2*PI*i/SAMPLE_RATE;
	}
}

/*
 * fuellen der Trackspuren mit Signalen
 */
void fill_toene (note* track, uint8_t trackCount) {

	uint8_t i = 0;
	//uint8_t array[] = {10,50,69,80,85,90,95,100,105,110,115,120,123,125,126,127};
	uint8_t a_freq[] = {69,90,105,126};
	uint16_t a_deltaTime[] = {100,1000,500,5555};    //Die Zeitdifferenz zum vorhergehenden Event. Es ist in variabler Länge encodiert.

	while (i < trackCount) {
		track[i].freq = create_note(a_freq[i]);	// TODO: muss noch angepasst werden...
		//track[i].counter = 0;
		track[i].deltaTime = a_deltaTime[i];
		i++;
	}
}

//void volume_up(void) {
//	volume_DSP = 1/(ton.noteOn) * VOLUME;
//    ton.noteOn--;
//}
//
//void volume_down(void) {
//
//	if (toggle > 0){
//		tmp= ton.noteOff;
//		toggle = 1;
//	}
//	volume_DSP = (ton.noteOff/tmp)*VOLUME;
//	ton.noteOff--;
//}
