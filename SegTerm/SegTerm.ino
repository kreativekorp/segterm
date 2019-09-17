#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <util/delay.h>
#include "KeyboardLib.h"
#include "SegTerm_buttons.h"
#include "SegTerm_config.h"
#include "SegTerm_display.h"
#include "SegTerm_eeprom.h"
#include "SegTerm_rtc.h"
#include "SegTerm_vt100.h"
#include "STApp_calendar.h"
#include "STApp_clock.h"
#include "STApp_terminal.h"
#include "TinyI2C.h"

void setup() {
	randomSeed(analogRead(2));
	initKeyboard();
	initButtons();
	initDisplay();
	loadDisplaySettingsFromEEPROM();
	initRTC();
	initVT100();
	if (getButtons() & BUTTON_2) vtPrint("\x1B[7;9y");
}

void loop() {
	if (calendar_setup()) {
		while (calendar_loop());
		calendar_quit();
	}
}
