#ifndef __STAPP_PIANO_H__
#define __STAPP_PIANO_H__

#include <Arduino.h>

// Arduino Mega D8
#define SPEAKER_PIN  8
#define SPEAKER_PORT PORTH
#define SPEAKER_MASK 0x20

// This technically can do polyphony but it sounds horrible.
// Maybe using a real speaker instead of a piezo would sound better?
#define POLYPHONY 8

// Must be a divisor of the 16MHz system clock.
// 32kHz is pretty much the only common sample rate
// of reasonable quality that divides into 16MHz.
#define SAMPLE_RATE 32000

bool piano_setup();
bool piano_loop();
void piano_quit();

#endif
