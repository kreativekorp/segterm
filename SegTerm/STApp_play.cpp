#include <Arduino.h>
#include <inttypes.h>
#include "KeyboardLib.h"
#include "PlayduinoLib.h"
#include "SegTerm_buttons.h"
#include "SegTerm_config.h"
#include "SegTerm_display.h"
#include "SegTerm_vt100.h"
#include "STApp_play.h"

static char buffer[PLAY_BUFLEN];
static int bufPtr;

static uint8_t getInput() {
	uint8_t ch;
	if ((ch = readKeyboardEvent())) {
		if (ch & KBD_PRESSED) {
			uint8_t mod = getKeyboardModifiers();
			ch = keyboardEventToASCII(ch, mod);
			return ch;
		}
	}
	if (Serial.available() > 0) {
		return Serial.read();
	}
	if (getButtons() & BUTTON_2) {
		delay(KBD_DEBOUNCE_DELAY);
		while (getButtons() & BUTTON_2);
		delay(KBD_DEBOUNCE_DELAY);
		return 27;
	}
	return 0;
}

bool play_setup() {
	buffer[0] = 0;
	bufPtr = 0;
	Serial.begin(9600);
	clearRows(0, SEGTERM_ROWS);
	initVT100();
	return true;
}

bool play_loop() {
	uint8_t ch;
	while ((ch = getInput())) {
		if (ch >= 32 && ch < 127) {
			if (bufPtr < (PLAY_BUFLEN-1)) {
				vtPutChar(ch);
				buffer[bufPtr] = ch;
				++bufPtr;
				buffer[bufPtr] = 0;
			}
		} else if (ch == '\b') {
			if (bufPtr > 0) {
				if (vtGetCursorX()) vtPrint("\b \b");
				else vtPrint("\x1B[A\x1B[255C\b \b");
				--bufPtr;
				buffer[bufPtr] = 0;
			}
		} else if (ch == '\n' || ch == '\r') {
			vtPutChar('\r');
			vtPutChar('\n');
			play(SEGTERM_BELL_PIN, buffer);
			buffer[0] = 0;
			bufPtr = 0;
		} else if (ch == 27) {
			return false;
		}
	}
	vtIdle();
	displayIdle();
	return true;
}

void play_quit() {
	clearRows(0, SEGTERM_ROWS);
	initVT100();
	Serial.end();
}
