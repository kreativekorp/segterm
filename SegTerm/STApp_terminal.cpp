#include <Arduino.h>
#include <inttypes.h>
#include "KeyboardLib.h"
#include "SegTerm_buttons.h"
#include "SegTerm_config.h"
#include "SegTerm_display.h"
#include "SegTerm_vt100.h"
#include "STApp_terminal.h"

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

bool terminal_setup() {
	Serial.begin(TERMINAL_BAUD_RATE);
	clearRows(0, SEGTERM_ROWS);
	initVT100();
	return true;
}

bool terminal_loop() {
	uint8_t ch;
	while ((ch = readKeyboardEvent())) handleKeyboardEvent(ch);
	while (Serial.available() > 0) vtPutChar(Serial.read());
	while ((ch = vtGetChar())) Serial.write(ch);
	vtIdle();
	displayIdle();

	if (getButtons() & BUTTON_2) {
		delay(KBD_DEBOUNCE_DELAY);
		while (getButtons() & BUTTON_2);
		delay(KBD_DEBOUNCE_DELAY);
		return false;
	}
	return true;
}

void terminal_quit() {
	clearRows(0, SEGTERM_ROWS);
	initVT100();
	Serial.end();
}
