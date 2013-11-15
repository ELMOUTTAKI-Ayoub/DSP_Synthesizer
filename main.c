/**
 * DSP-Bibliotheken
 */
#include "dsk6713_aic23.h"  			//support file for codec,DSK
#include "dsk6713_led.h"
#include "dsk6713_dip.h"
#include "dsk6713config.h"
#include "dsk6713.h"

/**
 * C-Bibliotheken
 */
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "notes.h"
#include "convert.h"

/**
 * Globale Variabelen
 */
DSK6713_AIC23_CodecHandle hCodec;
Uint32 		fs = DSK6713_AIC23_FREQ_44KHZ; 	// setzen der Sampling-Rate (8, 16, 44, 96 kHz)

//Uint16 		eventId1;
midiTracks 	midiSample;
trackChunk 	midiTrack[16];
command 	midiCommands[16*200];
command* 	midiCommandsPointer[16];
short 		counter = 0;
short 		index = 0;
float 		volume_DSP = 1.0f;			// external DSP-Board Volume
//short oneTick = 0;
//float timePerTick = 0;
short 		sinus[SAMPLE_RATE]; 		// Lockup Tabelle
uint8_t 	tmp, i = 0;

/**
 * Marc's declatation
 */
//uint8_t trackCount = 1;
/***************************/

/**
 * Externe Funktion
 */
extern far void vectors();

/**
 * Interrupt Service Routine
 */
interrupt void c_int11(void){
	static uint8_t 	t_timeDevision=0;
	uint8_t 		t_index = 0;
	

	if(t_timeDevision>15){
		while (t_index < midiSample.numTracks) {
			if (midiSample.tracks[t_index].delayCounter == 0) {
				if(	midiSample.tracks[t_index].commandCounter < midiSample.tracks[t_index].size.size-1){
					midiSample.tracks[t_index].commandCounter++;
				}
				if(	midiSample.tracks[t_index].commandCounter+1 < midiSample.tracks[t_index].size.size){
					midiSample.tracks[t_index].delayCounter = midiSample.commands[t_index][midiSample.tracks[t_index].commandCounter+1].deltaTime;
				}
			}
			else{
				midiSample.tracks[t_index].delayCounter--;
			}

			t_index++;
		}
		t_timeDevision=0;
	}
	t_timeDevision++;

	return;							//return from interrupt
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


int main(int argc, char **argv) {

	union {Uint32 combo;short channel[2];} AIC23_data;

	headerChunk headerSample;
	trackChunk trackSample;

	midiSample.tracks = midiTrack;
	uint8_t t_index=0;
	while(t_index<16){
		midiCommandsPointer[t_index]=&midiCommands[200*t_index];
		t_index++;
	}
	midiSample.commands = midiCommandsPointer;

	create_lut_sinus();		// fill the Lookup Table with the Sinus-signal
	convertMidi(argc, argv, &headerSample, &trackSample);						// Einlesen einer Midi-Datei

	//oneTick = headerSample.field.field[0] / 32;  								// 1Tick = ( TimeDevision / 1/4Note ) * 1/128Note
	//timePerTick = oneTick * (QUARTER_NOTE_TIME / headerSample.field.field[0]);	// Laenge einer 1/128 Note

	fill_toene(&midiSample);
	convert_delta_time(&midiSample);
	convert_note_number(&midiSample);	// Umwandlung der

	DSK6713_init(); 		// call BSL to init DSK-EMIF,PLL)
	interupt_config();		// configuration the Interupts
	DSK6713_LED_init();		// init LED from BSL

	while (1) {
		//read next value from codec (poll until value available)
		while (!DSK6713_AIC23_read(hCodec, &AIC23_data.combo));
		AIC23_data.combo=0;
		i=0;
		while (i < midiSample.numTracks) {
			uint16_t t_data = sinus[midiSample.tracks[i].counter] >> 4;
			AIC23_data.channel[LEFT] += t_data;
			AIC23_data.channel[RIGHT] += t_data;
			// wenn ... und Note ON
			if(midiSample.tracks[i].commandCounter!=-1 && midiSample.commands[i][midiSample.tracks[i].commandCounter].command == NOTE_ON){
				// signal processing

				midiSample.tracks[i].counter += midiSample.commands[i][midiSample.tracks[i].commandCounter].noteNumber;
				if (midiSample.tracks[i].counter > SAMPLE_RATE-1)
					midiSample.tracks[i].counter -= SAMPLE_RATE;
			}
			i++;
		}
		DSK6713_AIC23_write(hCodec, AIC23_data.combo);
	}
	//return EXIT_SUCCESS;
}

