#include <Arduino.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include "SegTerm_buttons.h"
#include "SegTerm_config.h"
#include "SegTerm_display.h"
#include "SegTerm_vt100.h"
#include "SlowSoftI2CMaster.h"

void setup() {
	initButtons();
	initDisplay();
	loadDisplaySettingsFromEEPROM(0);
	initVT100();
	if (getButtons() & BUTTON_2) vtPrint("\x1B[7;9y");
	Serial.begin(9600);
}

void loop() {
	uint8_t ch;
	while (Serial.available() > 0) vtPutChar(Serial.read());
	while ((ch = vtGetChar())) Serial.write(ch);
	vtIdle();
	displayIdle();
}
