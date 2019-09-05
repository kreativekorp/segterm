#include <Arduino.h>
#include <util/delay.h>
#include "KeyboardLib.h"

char noteNames[] = {
	'A',  0,0, 'A','#',0, 'B',0,0, 'C',  0,0, 'C','#',0, 'D',  0,0,
	'D','#',0, 'E',  0,0, 'F',0,0, 'F','#',0, 'G',  0,0, 'G','#',0,
};

uint8_t shifted = 0;

void setup() {
	initKeyboard();
	Serial.begin(9600);
}

void loop() {
	uint8_t ch;
	char ach[4];
	ach[0] = ' ';
	ach[1] = ' ';
	ach[2] = ' ';
	ach[3] = 0;
	while ((ch = readKeyboardEvent())) {
		if (ch == (KBD_KEY_SHIFT | KBD_PRESSED )) shifted = 1;
		if (ch == (KBD_KEY_SHIFT | KBD_RELEASED)) shifted = 0;
		if (ch & KBD_PRESSED ) Serial.print("PRESSED  ");
		if (ch & KBD_RELEASED) Serial.print("RELEASED ");
		ch &= KBD_KEY;
		Serial.print(ch >> 4, HEX);
		Serial.print(ch & 15, HEX);
		ach[1] = keyboardEventToASCII(ch, shifted);
		if (ach[1] < 32 || ach[1] >= 127) ach[1] = ' ';
		Serial.print(ach);
		Serial.print(&noteNames[(ch % 12) * 3]);
		Serial.println((ch + 21) / 12);
	}
}
