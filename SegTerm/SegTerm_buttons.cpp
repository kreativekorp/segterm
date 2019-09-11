#include <Arduino.h>
#include <inttypes.h>
#include "SegTerm_buttons.h"

void initButtons() {
	BUTTON_1_DDR  &=~ BUTTON_1_MASK;
	BUTTON_1_PORT |=  BUTTON_1_MASK;
	BUTTON_2_DDR  &=~ BUTTON_2_MASK;
	BUTTON_2_PORT |=  BUTTON_2_MASK;
	BUTTON_3_DDR  &=~ BUTTON_3_MASK;
	BUTTON_3_PORT |=  BUTTON_3_MASK;
}

uint8_t getButtons() {
	uint8_t state = 0;
	if (!(BUTTON_1_PIN & BUTTON_1_MASK)) state |= BUTTON_1;
	if (!(BUTTON_2_PIN & BUTTON_2_MASK)) state |= BUTTON_2;
	if (!(BUTTON_3_PIN & BUTTON_3_MASK)) state |= BUTTON_3;
	return state;
}
