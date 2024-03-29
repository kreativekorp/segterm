#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <util/delay.h>
#include "KeyboardLib.h"
#include "PlayduinoLib.h"
#include "SegTerm_buttons.h"
#include "SegTerm_config.h"
#include "SegTerm_display.h"
#include "SegTerm_eeprom.h"
#include "SegTerm_rtc.h"
#include "SegTerm_vt100.h"
#include "STApp_calendar.h"
#include "STApp_clock.h"
#include "STApp_launcher.h"
#include "STApp_piano.h"
#include "STApp_play.h"
#include "STApp_settings.h"
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
	
	uint8_t ch;
	if ((ch = getButtons())) {
		switch (ch) {
			case BUTTON_1: // launcher
				EEPROMwrite(EE_CURRAPP, 255);
				break;
			case BUTTON_2: // settings
				EEPROMwrite(EE_CURRAPP, 192);
				break;
			case BUTTON_3: // terminal
				EEPROMwrite(EE_CURRAPP, 200);
				break;
			case (BUTTON_1|BUTTON_2): // calendar
				EEPROMwrite(EE_CURRAPP, 33);
				break;
			case (BUTTON_1|BUTTON_3): // self test
				vtPrint("\x1B[7;9y");
				break;
			case (BUTTON_1|BUTTON_2|BUTTON_3): // factory reset
				EEPROMwrite(EE_CURRAPP, 255);
				loadDefaultDisplaySettings();
				saveDisplaySettingsToEEPROM();
				vtSetMode(VT100_MODE_DEFAULT);
				vtSaveMode();
				break;
		}
		delay(KBD_DEBOUNCE_DELAY);
		while (getButtons() & ch);
		delay(KBD_DEBOUNCE_DELAY);
	}
}

void loop() {
	if (launcher_setup()) {
		while (launcher_loop());
		launcher_quit();
	}
}
