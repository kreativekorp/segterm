#ifndef __STAPP_PIANO_H__
#define __STAPP_PIANO_H__

#include <Arduino.h>

// Arduino Mega D8
#define SPEAKER_PIN  8
#define SPEAKER_PORT PORTH
#define SPEAKER_MASK 0x20

// Quality deteriorates rapidly the more voices we add.
#define POLYPHONY 8

// Must be a divisor of the 16MHz system clock.
// 32kHz is pretty much the only common sample rate
// of reasonable quality that divides into 16MHz.
#define SAMPLE_RATE 32000

// Individual notes sound best with this at 2,
// but polyphony sounds best with this at 10.
#define DUTY_CYCLE_DIVISOR 10

bool piano_setup();
bool piano_loop();
void piano_quit();

#endif
