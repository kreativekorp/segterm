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
#include "STApp_terminal.h"
#include "TinyI2C.h"

void setup() {
	initKeyboard();
	initButtons();
	initDisplay();
	loadDisplaySettingsFromEEPROM(0);
	initVT100();
	if (getButtons() & BUTTON_2) vtPrint("\x1B[7;9y");
}

void loop() {
	if (terminal_setup()) {
		while (terminal_loop());
		terminal_quit();
	}
}
