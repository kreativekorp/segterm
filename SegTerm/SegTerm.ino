#include <Arduino.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include "SegTerm_config.h"
#include "SegTerm_display.h"
#include "SegTerm_vt100.h"
#include "SlowSoftI2CMaster.h"

void setup() {
	initDisplay();
	loadDisplaySettingsFromEEPROM(0);
	initVT100();
	Serial.begin(9600);
}

void loop() {
	uint8_t ch;
	while (Serial.available() > 0) vtPutChar(Serial.read());
	while ((ch = vtGetChar())) Serial.write(ch);
	vtIdle();
	displayIdle();
}
