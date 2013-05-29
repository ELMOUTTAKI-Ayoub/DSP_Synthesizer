/*
 * DSP-Bibliotheken
 */
#include "dsk6713_aic23.h"  			//support file for codec,DSK
#include "dsk6713_led.h"
#include "dsk6713_dip.h"
#include "dsk6713config.h"
#include "dsk6713.h"

/*
 * C-Bibliotheken
 */
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "notes.h"
#include "convert.h"


DSK6713_AIC23_CodecHandle hCodec;
Uint32 fs = DSK6713_AIC23_FREQ_8KHZ; 	//set sampling rate (8, 16, 44, 96 kHz)
Uint16 eventId1;

short counter = 0;
short index = 0;
float volume_DSP = 0.5f;
short oneTick = 0;
float timePerTick = 0;
Uint8 FreqDivider = 125; 			// FreqDivider = 8K * timePerTick
note *track;
short sinus[8000]; 			//lockup table

/*
 * Marc's declatation
 */
uint8_t trackCount = 1;

extern far void vectors();         //external function

/*
 * Aller 1/128 Note wird diese Funktion ausgeführt
 */
void control_time(){

	DSK6713_LED_toggle(2);

}

interrupt void c_int11(void){  		//interrupt service routine
	
//	if (FreqDivider == 0){
//		control_time();
//		FreqDivider = 125;
//	}else{
//		FreqDivider--;
//	}

	return;									//return from interrupt
}

/*
 *  Accessory functions
 */
void interupt_config() {

	int *p;
	//short press = 0;
	//----------------------------------------- Set up needed to for interrupts ----------------------------------------
	//IRQ_globalDisable(); //disable interrupts with CSL
	CSR = CSR & 0xFFFFFFFFFE; //disable interrupts without CSL
	CSR &= ~0x1; //disable interrupts without CSL

	hAIC23_handle=DSK6713_AIC23_openCodec(0, &config); // handle(pointer) to codec
	DSK6713_AIC23_setFreq(hAIC23_handle, fs); // set sample rate
	MCBSP_config(DSK6713_AIC23_DATAHANDLE,&AIC23CfgData); // interface 32 bits to AIC23
	MCBSP_start(DSK6713_AIC23_DATAHANDLE, MCBSP_XMIT_START | MCBSP_RCV_START |
	MCBSP_SRGR_START | MCBSP_SRGR_FRAMESYNC, 220); // start data channel
	//CODECEventId=MCBSP_getXmtEventId(DSK6713_AIC23_DATAHANDLE); //McBSP1 Xmit
	// CODECEventId=0x0E;		//xint1 interrupt selector value (see datasheet page 70, table 28
							// or tab. 14-3 on page 14-4 of peripheral device guide
	IRQ_setVecs(vectors);     			//point to the IRQ vector table
	IRQ_map(CODECEventId, 11); //map McBSP1 Xmit to INT11
	p=(int*)(MUXH); //adresse von MUXH - ohne Typecasting funktioniert das nicht...
	//set MUXH bits 9:5 (belonging to int11) to 01110 (specifying that it belongs to Xint1
	//*p=*p & 0b1111,1111,1111,1111,1111,1100,0001,1111;
	*p = *p & 0xFFFFFC1F;
	//*p=*p | 0b0000,0000,0000,0000,0000,0001,1100,0000;
	*p = *p | 0x1C0;

	//IRQ_reset(CODECEventId); //reset codec INT 11
	ICR = 0x800; // clear interrupt flag at bit 11
	IER &= ~0x800; //disable interrupt 11

	//IRQ_globalEnable(); //globally enable interrupts
	CSR |= 0x1;  //set Control-and-Status-Register (CSR) bit 0 to 1

	//IRQ_nmiEnable(); //enable NMI interrupt
	IER |=0x2;

	//IRQ_enable(CODECEventId); //enable CODEC eventXmit INT11
	IER |= 0x800; //enable interrupt 11

	MCBSP_write(DSK6713_AIC23_DATAHANDLE,0); //start McBSP interrupt
}


void main(int argc, char **argv) {

	uint8_t i = 0;

	DSK6713_init(); 						// call BSL to init DSK-EMIF,PLL)
	interupt_config();
	DSK6713_LED_init();						//init LED from BSL
	DSK6713_DIP_init();

	create_lut_sinus();						//fill the Lookup Table with the Sinus-signal

	headerChunk headerSample;
	trackChunk trackSample;
	convertMidi(argc, argv, &headerSample, &trackSample);						//Einlesen einer Midi-Datei
	//printf("\nTRACK:\tNummer %u gefunden, mit einer Länge von %i-Bytes", trackSample->size.size);

	oneTick = headerSample.field.field[0] / 32;  								// 1Tick = ( TimeDevision / 1/4Note ) * 1/128Note
	timePerTick = oneTick * (QUARTER_NOTE_TIME / headerSample.field.field[0]);	// Länge einer 1/128 Note
	//trackCount = headerSample.field.field[1];									// Anzahl der Tracks in einer MIDI-Datei
	trackCount = 3;
	track = malloc(trackCount*sizeof(int));										// Anlegen des Track-Arrays

	if( NULL == track ) {
	      printf("\nFehler: Es konnten keine Tracks angelegt werden. Mögliche Ursache: Kein freier Speicherplatz vorhanden.");
	  //    return EXIT_FAILURE;
	}

	//füllen der Trackspuren mit Signalen
//	while (i < trackCount) {
//		track[i].freq = create_note(69);	// TODO: muss noch angepasst werden...
//		i++;
//	}

	track[0].freq = create_note(69);
	track[1].freq = create_note(100);
	track[2].freq = create_note(80);

	union {Uint32 combo;short channel[2];} AIC23_data;

//	track[0].counter = 0;
//	track[1].counter = 0;
//	track[2].counter = 0;

	i=0;
	while (1) {
		while (!DSK6713_AIC23_read(hCodec, &AIC23_data.combo));
		AIC23_data.combo=0;
		i=0;
		while (i < trackCount) {
			//read next value from codec (poll until value available)
			// signal processing
			AIC23_data.channel[LEFT] += volume_DSP*(sinus[track[i].counter])/trackCount;
			AIC23_data.channel[RIGHT] += volume_DSP*(sinus[track[i].counter])/trackCount;
			track[i].counter +=(short)track[i].freq;
			if (track[i].counter > (8000-1)) track[i].counter -= 8000;
			i++;
		}
		DSK6713_AIC23_write(hCodec, AIC23_data.combo);
	}
	//return EXIT_SUCCESS;
}

