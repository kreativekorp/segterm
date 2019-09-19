#include <Arduino.h>
#include <EEPROM.h>
#include <inttypes.h>
#include "KeyboardLib.h"
#include "SegTerm_buttons.h"
#include "SegTerm_config.h"
#include "SegTerm_display.h"
#include "SegTerm_eeprom.h"
#include "STApp_launcher.h"

// -------- START ADDING APPS HERE -------- //

#include "STApp_clock.h"
#include "STApp_calendar.h"
#include "STApp_play.h"
#include "STApp_piano.h"
#include "STApp_terminal.h"

APPNAMES
	APPNAME( 32, "Clock")
	APPNAME( 33, "Calendar")
	APPNAME(160, "Play Notation")
	APPNAME(161, "Play Piano")
	APPNAME(200, "Terminal")
ENDNAMES

REGAPPS
	REGAPP( 32, clock)
	REGAPP( 33, calendar)
	REGAPP(160, play)
	REGAPP(161, piano)
	REGAPP(200, terminal)
ENDAPPS

// -------- STOP ADDING APPS HERE -------- //

static uint8_t currApp;

static void drawMenuItem(uint8_t row, uint8_t curr, const char * name) {
	uint8_t col = 0;
	setCharAndAttr(row, col, (curr ? 0x40 : 0), SEGTERM_CHATTR_RAW); col++;
	setCharAndAttr(row, col, (curr ? 0x70 : 0), SEGTERM_CHATTR_RAW); col++;
	while (col < (SEGTERM_COLS<<2) && *name) {
		setChar(row, col, *name);
		col++; name++;
	}
	while (col < (SEGTERM_COLS<<2)) {
		setChar(row, col, 0);
		col++;
	}
}

static void drawMenu() {
	uint8_t row, i;
	const char * name;
	
	row = ((SEGTERM_ROWS - 1) >> 1);
	name = appName((i = currApp));
	drawMenuItem(row, 1, name);
	
	row = ((SEGTERM_ROWS - 1) >> 1) - 1;
	i = currApp - 1;
	while (row < SEGTERM_ROWS && i > 0) {
		if ((name = appName(i))) {
			drawMenuItem(row, 0, name);
			row--;
		}
		i--;
	}
	if (row < SEGTERM_ROWS) clearRows(0, row + 1);
	
	row = ((SEGTERM_ROWS - 1) >> 1) + 1;
	i = currApp + 1;
	while (row < SEGTERM_ROWS && i < 255) {
		if ((name = appName(i))) {
			drawMenuItem(row, 0, name);
			row++;
		}
		i++;
	}
	if (row < SEGTERM_ROWS) clearRows(row, SEGTERM_ROWS);
}

static void menuStart() {
	for (;;) { if (appName(currApp)) break; currApp++; }
	drawMenu();
}

static void menuUp() {
	for (;;) { currApp--; if (appName(currApp)) break; }
	drawMenu();
}

static void menuDown() {
	for (;;) { currApp++; if (appName(currApp)) break; }
	drawMenu();
}

bool launcher_setup() {
	clearRows(0, SEGTERM_ROWS);
	currApp = EEPROM.read(EE_CURRAPP);
	runApp(currApp);
	clearRows(0, SEGTERM_ROWS);
	EEPROMwrite(EE_CURRAPP, 255);
	menuStart();
	return true;
}

bool launcher_loop() {
	uint8_t ch, launch = 0;
	if ((ch = readKeyboardEvent())) {
		if (ch & KBD_PRESSED) {
			ch &= KBD_KEY;
			switch (ch) {
				case KBD_KEY_ENTER: launch = 1; break;
				case KBD_KEY_SPACE: launch = 1; break;
				case KBD_KEY_LEFT:  menuUp();   break;
				case KBD_KEY_RIGHT: menuDown(); break;
				case KBD_KEY_UP:    menuUp();   break;
				case KBD_KEY_DOWN:  menuDown(); break;
				default: return true;
			}
			ch |= KBD_RELEASED;
			while (readKeyboardEvent() != ch);
		}
	}
	if ((ch = getButtons())) {
		switch (ch) {
			case BUTTON_1: menuUp();   break;
			case BUTTON_2: launch = 1; break;
			case BUTTON_3: menuDown(); break;
		}
		delay(KBD_DEBOUNCE_DELAY);
		while (getButtons() & ch);
		delay(KBD_DEBOUNCE_DELAY);
	}
	if (launch) {
		clearRows(0, SEGTERM_ROWS);
		EEPROMwrite(EE_CURRAPP, currApp);
		runApp(currApp);
		clearRows(0, SEGTERM_ROWS);
		EEPROMwrite(EE_CURRAPP, 255);
		drawMenu();
	}
	return true;
}

void launcher_quit() {
	clearRows(0, SEGTERM_ROWS);
}
