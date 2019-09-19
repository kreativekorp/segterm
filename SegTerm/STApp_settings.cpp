#include <Arduino.h>
#include <EEPROM.h>
#include <inttypes.h>
#include "KeyboardLib.h"
#include "SegTerm_buttons.h"
#include "SegTerm_config.h"
#include "SegTerm_display.h"
#include "SegTerm_eeprom.h"
#include "SegTerm_vt100.h"
#include "STApp_settings.h"

static void printLeft(uint8_t row, uint8_t col, const char * s) {
	while (col < (SEGTERM_COLS<<2) && *s) {
		setCharAndAttr(row, col, *s, 0);
		++col; ++s;
	}
}

static void printRight(uint8_t row, uint8_t col, const char * s) {
	if (col < (SEGTERM_COLS<<2)) {
		const char * e = s; while (*e) ++e;
		col = (SEGTERM_COLS<<2) - col;
		while (col > 0 && e != s) {
			--e; --col;
			setCharAndAttr(row, col, *e, 0);
		}
	}
}

static void clearCenter(uint8_t row, uint8_t lcols, uint8_t rcols) {
	if (lcols + rcols < (SEGTERM_COLS<<2)) {
		clearCols(row, lcols, (SEGTERM_COLS<<2) - rcols);
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

static uint8_t dateRow;
static uint8_t timeRow;
static uint8_t loadedFont;
static uint8_t firstDay;
static uint8_t currSetting;

static void drawMenuItem(uint8_t row, uint8_t curr, uint8_t i) {
	uint8_t arrow = false;
	setCharAndAttr(row, 0, (curr ? 0x40 : 0), SEGTERM_CHATTR_RAW);
	setCharAndAttr(row, 1, (curr ? 0x70 : 0), SEGTERM_CHATTR_RAW);
	switch (i) {
		case SETTING_BRIGHTNESS:
			printLeft(row, 2, "Brightness");
			clearCenter(row, 12, 2);
			itoa(getBrightness());
			printRight(row, 1, itoabuf);
			arrow = true;
			break;
		case SETTING_FONT:
			printLeft(row, 2, "Font");
			clearCenter(row, 6, 6);
			switch (loadedFont) {
				case 0:  printRight(row, 0, "Readable"); break;
				case 1:  printRight(row, 0, "Distinct"); break;
				default: printRight(row, 0, "Custom"  ); break;
			}
			break;
		case SETTING_LOWERCASE:
			printLeft(row, 2, "Lowercase");
			clearCenter(row, 11, 2);
			printRight(row, 0, getLcMode() ? "On" : "Off");
			break;
		case SETTING_DOT_MODE:
			printLeft(row, 2, "Dot Mode");
			clearCenter(row, 10, 3);
			switch (getDotMode()) {
				case SEGTERM_DOTMODE_HIGHBIT:
					printRight(row, 0, "MSB"  ); break;
				case SEGTERM_DOTMODE_UPPERCASE:
					printRight(row, 0, "Upper"); break;
				case SEGTERM_DOTMODE_LOWERCASE:
					printRight(row, 0, "Lower"); break;
				default:
					printRight(row, 0, "Off"  ); break;
			}
			break;
		case SETTING_LOCAL_ECHO:
			printLeft(row, 2, "Local Echo");
			clearCenter(row, 12, 2);
			printRight(row, 0, (vtGetMode() & VT100_MODE_LOCAL_ECHO) ? "On" : "Off");
			break;
		case SETTING_CURSOR:
			printLeft(row, 2, "Cursor");
			clearCenter(row, 8, 2);
			printRight(row, 0, (
				(vtGetMode() & VT100_MODE_CURSOR_ON) ?
				(
					(vtGetMode() & VT100_MODE_CURSOR_BLINK) ?
					"Blink" :
					"On"
				) :
				"Off"
			));
			break;
		case SETTING_DATE:
			printLeft(row, 2, "Date");
			clearCenter(row, 6, 1);
			dateRow = row;
			arrow = true;
			break;
		case SETTING_TIME:
			printLeft(row, 2, "Time");
			clearCenter(row, 6, 1);
			timeRow = row;
			arrow = true;
			break;
		case SETTING_FIRST_DAY:
			printLeft(row, 2, "First Day");
			clearCenter(row, 11, 3);
			switch (firstDay) {
				case 1:  printRight(row, 0, "Mon"); break;
				case 2:  printRight(row, 0, "Tue"); break;
				case 3:  printRight(row, 0, "Wed"); break;
				case 4:  printRight(row, 0, "Thu"); break;
				case 5:  printRight(row, 0, "Fri"); break;
				case 6:  printRight(row, 0, "Sat"); break;
				default: printRight(row, 0, "Sun"); break;
			}
			break;
		case SETTING_ABOUT:
			printLeft(row, 2, "About");
			clearCenter(row, 7, 1);
			arrow = true;
			break;
		case SETTING_MAX:
			printLeft(row, 2, "Exit");
			clearCenter(row, 6, 0);
			break;
	}
	if (arrow) setCharAndAttr(row, ((SEGTERM_COLS<<2)-1), 0x70, SEGTERM_CHATTR_RAW);
}

static void drawMenu() {
	uint8_t row = ((SEGTERM_ROWS - 1) >> 1);
	uint8_t i = currSetting;
	drawMenuItem(row, 1, i);
	
	row = ((SEGTERM_ROWS - 1) >> 1) - 1;
	i = currSetting - 1;
	while (row < SEGTERM_ROWS && i <= SETTING_MAX) {
		drawMenuItem(row, 0, i); --row; --i;
	}
	if (row < SEGTERM_ROWS) clearRows(0, row + 1);
	
	row = ((SEGTERM_ROWS - 1) >> 1) + 1;
	i = currSetting + 1;
	while (row < SEGTERM_ROWS && i <= SETTING_MAX) {
		drawMenuItem(row, 0, i); ++row; ++i;
	}
	if (row < SEGTERM_ROWS) clearRows(row, SEGTERM_ROWS);
}

static void menuStart() {
	dateRow = 255;
	timeRow = 255;
	loadedFont = identifyLoadedFont();
	firstDay = EEPROM.read(EE_FIRSTDAY);
	currSetting = 0;
	drawMenu();
}

static void menuUp() {
	dateRow = 255;
	timeRow = 255;
	if (--currSetting > SETTING_MAX) currSetting = SETTING_MAX;
	drawMenu();
}

static void menuDown() {
	dateRow = 255;
	timeRow = 255;
	if (++currSetting > SETTING_MAX) currSetting = 0;
	drawMenu();
}

static uint8_t menuActionDown() {
	switch (currSetting) {
		// TODO
		default:
			return SETTING_ACTION_NOOP;
	}
}

static uint8_t menuActionUp() {
	switch (currSetting) {
		// TODO
		case SETTING_ABOUT:
			about();
			return SETTING_ACTION_REDRAW_MENU;
		case SETTING_MAX:
			return SETTING_ACTION_EXIT;
		default:
			return SETTING_ACTION_NOOP;
	}
}

static bool menuResult(uint8_t action) {
	switch (action) {
		case SETTING_ACTION_REDRAW_ITEM:
			drawMenuItem(
				((SEGTERM_ROWS - 1) >> 1),
				1, currSetting
			);
			return true;
		case SETTING_ACTION_REDRAW_MENU:
			drawMenu();
			return true;
		case SETTING_ACTION_EXIT:
			return false;
		default:
			return true;
	}
}

bool settings_setup() {
	clearRows(0, SEGTERM_ROWS);
	menuStart();
	return true;
}

bool settings_loop() {
	uint8_t ch, action = 0;
	if ((ch = readKeyboardEvent())) {
		if (ch & KBD_PRESSED) {
			ch &= KBD_KEY;
			if (ch == KBD_KEY_ENTER || ch == KBD_KEY_SPACE) {
				menuResult(menuActionDown());
			} else if (ch != KBD_KEY_ESC) switch (ch) {
				case KBD_KEY_LEFT:  menuUp();   break;
				case KBD_KEY_RIGHT: menuDown(); break;
				case KBD_KEY_UP:    menuUp();   break;
				case KBD_KEY_DOWN:  menuDown(); break;
				default: return true;
			}
			ch |= KBD_RELEASED;
			while (readKeyboardEvent() != ch);
			ch &= KBD_KEY;
			if (ch == KBD_KEY_ENTER || ch == KBD_KEY_SPACE) {
				return menuResult(menuActionUp());
			} else if (ch == KBD_KEY_ESC) {
				return false;
			}
		}
	}
	if ((ch = getButtons())) {
		if (ch & BUTTON_2) {
			menuResult(menuActionDown());
		} else switch (ch) {
			case BUTTON_1: menuUp();   break;
			case BUTTON_3: menuDown(); break;
		}
		delay(KBD_DEBOUNCE_DELAY);
		while (getButtons() & ch);
		delay(KBD_DEBOUNCE_DELAY);
		if (ch & BUTTON_2) {
			return menuResult(menuActionUp());
		}
	}
	if (SEGTERM_COLS >= 3) {
		if (dateRow < SEGTERM_ROWS) {
			// TODO
		}
		if (timeRow < SEGTERM_ROWS) {
			// TODO
		}
	}
	return true;
}

void settings_quit() {
	clearRows(0, SEGTERM_ROWS);
}
