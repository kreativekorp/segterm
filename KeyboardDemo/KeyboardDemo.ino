#include <Arduino.h>
#include <util/delay.h>
#include "KeyboardLib.h"

void setup() {
	initKeyboard();
	Serial.begin(9600);
}

void loop() {
	uint8_t ch;
	while ((ch = readKeyboardEvent())) {
		if (ch & KBD_PRESSED ) Serial.print("PRESSED  ");
		if (ch & KBD_RELEASED) Serial.print("RELEASED ");
		Serial.println(ch & KBD_KEY);
	}
}
