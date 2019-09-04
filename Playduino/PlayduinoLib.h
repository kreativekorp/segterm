#ifndef __PLAYDUINO_LIB_H__
#define __PLAYDUINO_LIB_H__

#include <inttypes.h>

// Pin on which tones are played.
#define PLAYDUINO_PIN         8

// The number of millis of silence between notes to make them distinct.
#define PLAYDUINO_NOTE_DELAY 10

unsigned int getNoteFrequency(uint8_t p);
void play(const char * notes);

#endif
