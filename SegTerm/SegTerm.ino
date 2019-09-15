#include <Arduino.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <util/delay.h>
#include "KeyboardLib.h"
#include "SegTerm_buttons.h"
#include "SegTerm_config.h"
#include "SegTerm_display.h"
#include "SegTerm_vt100.h"
#include "TinyI2C.h"

void setup() {
	initKeyboard();
	initButtons();
	initDisplay();
	loadDisplaySettingsFromEEPROM(0);
	initVT100();
	if (getButtons() & BUTTON_2) vtPrint("\x1B[7;9y");
	Serial.begin(9600);
}

static void handleKeyboardChar(uint8_t ch, uint8_t mode) {
	Serial.write(ch);
	if (mode & VT100_MODE_LOCAL_ECHO) vtPutChar(ch);
}

static void handleKeyboardEscSeq(uint8_t ch, uint8_t mode) {
	Serial.write(27);
	if (mode & VT100_MODE_LOCAL_ECHO) vtPutChar(27);

	if (!(mode & VT100_MODE_VT52)) {
		Serial.write('[');
		if (mode & VT100_MODE_LOCAL_ECHO) vtPutChar('[');
	}

	Serial.write(ch);
	if (mode & VT100_MODE_LOCAL_ECHO) vtPutChar(ch);
}

static void handleKeyboardEvent(uint8_t ch) {
	if (ch & KBD_PRESSED) {
		uint8_t mode = vtGetMode();
		if (!(mode & VT100_MODE_KEYBOARD_LOCK)) {
			uint8_t mod = getKeyboardModifiers();
			ch = keyboardEventToASCII(ch, mod);
			if (ch >= 32) handleKeyboardChar(ch, mode);
			else switch (ch) {
				case KBD_ASCII_BKSP:  handleKeyboardChar  (8,   mode); break;
				case KBD_ASCII_TAB:   handleKeyboardChar  (9,   mode); break;
				case KBD_ASCII_ESC:   handleKeyboardChar  (27,  mode); break;
				case KBD_ASCII_LEFT:  handleKeyboardEscSeq('D', mode); break;
				case KBD_ASCII_RIGHT: handleKeyboardEscSeq('C', mode); break;
				case KBD_ASCII_UP:    handleKeyboardEscSeq('A', mode); break;
				case KBD_ASCII_DOWN:  handleKeyboardEscSeq('B', mode); break;
				case KBD_ASCII_ENTER:
					if (mod & KBD_MOD_ALT) {
						handleKeyboardChar(13, mode);
						handleKeyboardChar(10, mode);
					} else if (mod & KBD_MOD_CTRL) {
						handleKeyboardChar(10, mode);
					} else {
						handleKeyboardChar(13, mode);
					}
					break;
			}
		}
	}
}

void loop() {
	uint8_t ch;
	while ((ch = readKeyboardEvent())) handleKeyboardEvent(ch);
	while (Serial.available() > 0) vtPutChar(Serial.read());
	while ((ch = vtGetChar())) Serial.write(ch);
	vtIdle();
	displayIdle();
}
