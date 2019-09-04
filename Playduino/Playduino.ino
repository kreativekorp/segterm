#include <Arduino.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include "PlayduinoLib.h"

#define BUFLEN 256
static char buffer[BUFLEN];
static int bufPtr;

void setup() {
	buffer[0] = 0;
	bufPtr = 0;
	Serial.begin(9600);
}

void loop() {
	uint8_t ch;
	while (Serial.available() > 0) {
		ch = Serial.read();
		if (ch == '\n' || ch == '\r') {
			play(buffer);
			buffer[0] = 0;
			bufPtr = 0;
		} else if (ch && bufPtr < (BUFLEN-1)) {
			buffer[bufPtr] = ch;
			++bufPtr;
			buffer[bufPtr] = 0;
		}
	}
}
