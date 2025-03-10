// Copyright 2015 <>< Charles Lohr under the ColorChord License.

#ifndef _EMBEDDEDOUT_H
#define _EMBEDDEDOUT_H

#include "ccconfig.h"  // Critical inclusion for NUM_LIN_LEDS

#ifdef __cplusplus
extern "C" {
#endif


//Controls brightness
#ifndef NOTE_FINAL_AMP
#define NOTE_FINAL_AMP  12   //Number from 0...255
#endif

//Controls, basically, the minimum size of the splotches.
#ifndef NERF_NOTE_PORP
#define NERF_NOTE_PORP 15 //value from 0 to 255
#endif

#ifndef NUM_LIN_LEDS
#define NUM_LIN_LEDS 32
#endif

#ifndef USE_NUM_LIN_LEDS
#define USE_NUM_LIN_LEDS NUM_LIN_LEDS
#endif


#ifndef LIN_WRAPAROUND
//Whether the output lights wrap around.
//(Can't easily run on embedded systems)
#define LIN_WRAPAROUND 0 
#endif

#ifndef SORT_NOTES
#define SORT_NOTES 0     //Whether the notes will be sorted. BUGGY Don't use.
#endif

// Declarations
extern uint8_t ledArray[];
extern uint8_t ledOut[NUM_LIN_LEDS * 3]; // Fixed array declaration
extern uint8_t RootNoteOffset; //Set to define what the root note is.  0 = A.

//For doing the nice linear strip LED updates
void UpdateLinearLEDs();

//For making all the LEDs the same and quickest.  Good for solo instruments?
void UpdateAllSameLEDs();

uint32_t ECCtoHEX( uint8_t note, uint8_t sat, uint8_t val );
uint32_t EHSVtoHEX( uint8_t hue, uint8_t sat, uint8_t val ); //hue = 0..255 // TODO: TEST ME!!!


#ifdef __cplusplus
}
#endif

#endif // _EMBEDDEDOUT_H
