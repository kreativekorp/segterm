#include <Arduino.h>
#include <inttypes.h>
#include "KeyboardLib.h"
#include "SegTerm_buttons.h"
#include "SegTerm_config.h"
#include "SegTerm_display.h"
#include "STApp_settings.h"

static void printLeft(uint8_t row, uint8_t col, const char * s) {
	while (col < (SEGTERM_COLS<<2) && *s) {
		setChar(row, col, *s); ++col; ++s;
	}
}

static void printRight(uint8_t row, uint8_t col, const char * s) {
	const char * e = s; while (*e) ++e;
	while (col > 0 && e != s) {
		--e; --col; setChar(row, col, *e);
	}
}

static char itoabuf[8];
static void itoa(int v) {
	int p0 = 0, p1 = 8;
	do { itoabuf[--p1] = ('0' | (v % 10)); v /= 10; } while (v);
	while (p1 < 8) { itoabuf[p0] = itoabuf[p1]; ++p0; ++p1; }
	itoabuf[p0] = 0;
}

static int freeRAM() {
	extern int __heap_start, * __brkval; int v;
	return (int)&v - ((__brkval == 0) ? (int)&__heap_start : (int)__brkval);
}

static void about() {
	uint8_t row;
	clearRows(0, SEGTERM_ROWS);
	itoa(freeRAM());
	if (SEGTERM_COLS >= 6) {
		row = (
			(SEGTERM_ROWS >= 6) ? ((SEGTERM_ROWS - 6) >> 1) :
			(SEGTERM_ROWS >= 4) ? ((SEGTERM_ROWS - 4) >> 1) :
			(SEGTERM_ROWS >= 2) ? ((SEGTERM_ROWS - 2) >> 1) : 0
		);
		printLeft(row, 0, "SegTerm by KreativeKorp"); ++row;
		if (SEGTERM_ROWS >= 2) {
			if (SEGTERM_ROWS >= 4) {
				if (SEGTERM_ROWS >= 6) ++row;
				printLeft(row, 0, "Sometimes even I have"); ++row;
				printLeft(row, 0, "to ask myself, \"why!?\""); ++row;
				if (SEGTERM_ROWS >= 6) ++row;
			}
			printLeft(row, 0, "Free RAM: ");
			printLeft(row, 10, itoabuf);
		}
	} else if (SEGTERM_COLS >= 4) {
		row = (
			(SEGTERM_ROWS >= 8) ? ((SEGTERM_ROWS - 8) >> 1) :
			(SEGTERM_ROWS >= 6) ? ((SEGTERM_ROWS - 6) >> 1) :
			(SEGTERM_ROWS >= 3) ? ((SEGTERM_ROWS - 3) >> 1) : 0
		);
		if (SEGTERM_ROWS >= 2) {
			printLeft(row, 0, "SegTerm by"); ++row;
			printLeft(row, 0, "KreativeKorp"); ++row;
			if (SEGTERM_ROWS >= 3) {
				if (SEGTERM_ROWS >= 6) {
					if (SEGTERM_ROWS >= 8) ++row;
					printLeft(row, 0, "Sometimes even"); ++row;
					printLeft(row, 0, "I have to ask"); ++row;
					printLeft(row, 0, "myself, \"why!?\""); ++row;
					if (SEGTERM_ROWS >= 8) ++row;
				}
				printLeft(row, 0, "Free RAM: ");
				printLeft(row, 10, itoabuf);
			}
		} else {
			printLeft(row, 0, "SegTerm by KK");
		}
	} else if (SEGTERM_COLS >= 2) {
		row = (
			(SEGTERM_ROWS >= 4) ? ((SEGTERM_ROWS - 4) >> 1) :
			(SEGTERM_ROWS >= 2) ? ((SEGTERM_ROWS - 2) >> 1) : 0
		);
		printLeft(row, 0, "SegTerm"); ++row;
		if (SEGTERM_ROWS >= 2) {
			printLeft(row, 1, "by KK"); ++row;
			if (SEGTERM_ROWS >= 4) {
				printLeft(row, 0, "Free RAM"); ++row;
				printLeft(row, 1, itoabuf);
			}
		}
	} else {
		row = (
			(SEGTERM_ROWS >= 7) ? ((SEGTERM_ROWS - 7) >> 1) :
			(SEGTERM_ROWS >= 4) ? ((SEGTERM_ROWS - 4) >> 1) :
			(SEGTERM_ROWS >= 2) ? ((SEGTERM_ROWS - 2) >> 1) : 0
		);
		if (SEGTERM_ROWS >= 2) {
			printLeft(row, 0, "Seg-"); ++row;
			printLeft(row, 0, "Term"); ++row;
			if (SEGTERM_ROWS >= 4) {
				printLeft(row, 1, "by"); ++row;
				printLeft(row, 1, "KK"); ++row;
				if (SEGTERM_ROWS >= 7) {
					printLeft(row, 0, "Free"); ++row;
					printLeft(row, 0, "RAM"); ++row;
					printLeft(row, 0, itoabuf);
				}
			}
		} else {
			printLeft(row, 0, "STKK");
		}
	}
	for (;;) {
		if (readKeyboardEvent() == (KBD_KEY_ESC | KBD_PRESSED)) {
			while (readKeyboardEvent() != (KBD_KEY_ESC | KBD_RELEASED));
			break;
		}
		if (getButtons() & BUTTON_2) {
			delay(KBD_DEBOUNCE_DELAY);
			while (getButtons() & BUTTON_2);
			delay(KBD_DEBOUNCE_DELAY);
			break;
		}
	}
	clearRows(0, SEGTERM_ROWS);
}

// TODO

bool settings_setup() {
	clearRows(0, SEGTERM_ROWS);
	return true;
}

bool settings_loop() {
	about();
	return false;
}

void settings_quit() {
	clearRows(0, SEGTERM_ROWS);
}
