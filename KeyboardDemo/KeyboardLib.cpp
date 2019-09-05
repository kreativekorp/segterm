#include <Arduino.h>
#include <util/delay.h>
#include "KeyboardLib.h"

void initKeyboard() {
	// Anodes are pulled high and read as inputs.
	KBD_ANODE_0_DDR  &=~ KBD_ANODE_0_MASK;
	KBD_ANODE_0_PORT |=  KBD_ANODE_0_MASK;
	KBD_ANODE_1_DDR  &=~ KBD_ANODE_1_MASK;
	KBD_ANODE_1_PORT |=  KBD_ANODE_1_MASK;
	KBD_ANODE_2_DDR  &=~ KBD_ANODE_2_MASK;
	KBD_ANODE_2_PORT |=  KBD_ANODE_2_MASK;
	KBD_ANODE_3_DDR  &=~ KBD_ANODE_3_MASK;
	KBD_ANODE_3_PORT |=  KBD_ANODE_3_MASK;
	KBD_ANODE_4_DDR  &=~ KBD_ANODE_4_MASK;
	KBD_ANODE_4_PORT |=  KBD_ANODE_4_MASK;
	KBD_ANODE_5_DDR  &=~ KBD_ANODE_5_MASK;
	KBD_ANODE_5_PORT |=  KBD_ANODE_5_MASK;
	KBD_ANODE_6_DDR  &=~ KBD_ANODE_6_MASK;
	KBD_ANODE_6_PORT |=  KBD_ANODE_6_MASK;
	KBD_ANODE_7_DDR  &=~ KBD_ANODE_7_MASK;
	KBD_ANODE_7_PORT |=  KBD_ANODE_7_MASK;
	// Cathodes are normally high but pulled low when probing.
	KBD_CATHODE_0_DDR  |= KBD_CATHODE_0_MASK;
	KBD_CATHODE_0_PORT |= KBD_CATHODE_0_MASK;
	KBD_CATHODE_1_DDR  |= KBD_CATHODE_1_MASK;
	KBD_CATHODE_1_PORT |= KBD_CATHODE_1_MASK;
	KBD_CATHODE_2_DDR  |= KBD_CATHODE_2_MASK;
	KBD_CATHODE_2_PORT |= KBD_CATHODE_2_MASK;
	KBD_CATHODE_3_DDR  |= KBD_CATHODE_3_MASK;
	KBD_CATHODE_3_PORT |= KBD_CATHODE_3_MASK;
	KBD_CATHODE_4_DDR  |= KBD_CATHODE_4_MASK;
	KBD_CATHODE_4_PORT |= KBD_CATHODE_4_MASK;
	KBD_CATHODE_5_DDR  |= KBD_CATHODE_5_MASK;
	KBD_CATHODE_5_PORT |= KBD_CATHODE_5_MASK;
	KBD_CATHODE_6_DDR  |= KBD_CATHODE_6_MASK;
	KBD_CATHODE_6_PORT |= KBD_CATHODE_6_MASK;
	KBD_CATHODE_7_DDR  |= KBD_CATHODE_7_MASK;
	KBD_CATHODE_7_PORT |= KBD_CATHODE_7_MASK;
}

void readKeyboardState(uint8_t * state) {
	state[0] = 0; state[1] = 0; state[2] = 0; state[3] = 0;
	state[4] = 0; state[5] = 0; state[6] = 0; state[7] = 0;
	
	KBD_CATHODE_0_PORT &=~ KBD_CATHODE_0_MASK;
	_delay_us(KBD_PROP_DELAY);
	if (!(KBD_ANODE_0_PIN & KBD_ANODE_0_MASK)) state[0] |= 0x80;
	if (!(KBD_ANODE_1_PIN & KBD_ANODE_1_MASK)) state[1] |= 0x80;
	if (!(KBD_ANODE_2_PIN & KBD_ANODE_2_MASK)) state[2] |= 0x80;
	if (!(KBD_ANODE_3_PIN & KBD_ANODE_3_MASK)) state[3] |= 0x80;
	if (!(KBD_ANODE_4_PIN & KBD_ANODE_4_MASK)) state[4] |= 0x80;
	if (!(KBD_ANODE_5_PIN & KBD_ANODE_5_MASK)) state[5] |= 0x80;
	if (!(KBD_ANODE_6_PIN & KBD_ANODE_6_MASK)) state[6] |= 0x80;
	if (!(KBD_ANODE_7_PIN & KBD_ANODE_7_MASK)) state[7] |= 0x80;
	KBD_CATHODE_0_PORT |=  KBD_CATHODE_0_MASK;
	
	KBD_CATHODE_1_PORT &=~ KBD_CATHODE_1_MASK;
	_delay_us(KBD_PROP_DELAY);
	if (!(KBD_ANODE_0_PIN & KBD_ANODE_0_MASK)) state[0] |= 0x40;
	if (!(KBD_ANODE_1_PIN & KBD_ANODE_1_MASK)) state[1] |= 0x40;
	if (!(KBD_ANODE_2_PIN & KBD_ANODE_2_MASK)) state[2] |= 0x40;
	if (!(KBD_ANODE_3_PIN & KBD_ANODE_3_MASK)) state[3] |= 0x40;
	if (!(KBD_ANODE_4_PIN & KBD_ANODE_4_MASK)) state[4] |= 0x40;
	if (!(KBD_ANODE_5_PIN & KBD_ANODE_5_MASK)) state[5] |= 0x40;
	if (!(KBD_ANODE_6_PIN & KBD_ANODE_6_MASK)) state[6] |= 0x40;
	if (!(KBD_ANODE_7_PIN & KBD_ANODE_7_MASK)) state[7] |= 0x40;
	KBD_CATHODE_1_PORT |=  KBD_CATHODE_1_MASK;
	
	KBD_CATHODE_2_PORT &=~ KBD_CATHODE_2_MASK;
	_delay_us(KBD_PROP_DELAY);
	if (!(KBD_ANODE_0_PIN & KBD_ANODE_0_MASK)) state[0] |= 0x20;
	if (!(KBD_ANODE_1_PIN & KBD_ANODE_1_MASK)) state[1] |= 0x20;
	if (!(KBD_ANODE_2_PIN & KBD_ANODE_2_MASK)) state[2] |= 0x20;
	if (!(KBD_ANODE_3_PIN & KBD_ANODE_3_MASK)) state[3] |= 0x20;
	if (!(KBD_ANODE_4_PIN & KBD_ANODE_4_MASK)) state[4] |= 0x20;
	if (!(KBD_ANODE_5_PIN & KBD_ANODE_5_MASK)) state[5] |= 0x20;
	if (!(KBD_ANODE_6_PIN & KBD_ANODE_6_MASK)) state[6] |= 0x20;
	if (!(KBD_ANODE_7_PIN & KBD_ANODE_7_MASK)) state[7] |= 0x20;
	KBD_CATHODE_2_PORT |=  KBD_CATHODE_2_MASK;
	
	KBD_CATHODE_3_PORT &=~ KBD_CATHODE_3_MASK;
	_delay_us(KBD_PROP_DELAY);
	if (!(KBD_ANODE_0_PIN & KBD_ANODE_0_MASK)) state[0] |= 0x10;
	if (!(KBD_ANODE_1_PIN & KBD_ANODE_1_MASK)) state[1] |= 0x10;
	if (!(KBD_ANODE_2_PIN & KBD_ANODE_2_MASK)) state[2] |= 0x10;
	if (!(KBD_ANODE_3_PIN & KBD_ANODE_3_MASK)) state[3] |= 0x10;
	if (!(KBD_ANODE_4_PIN & KBD_ANODE_4_MASK)) state[4] |= 0x10;
	if (!(KBD_ANODE_5_PIN & KBD_ANODE_5_MASK)) state[5] |= 0x10;
	if (!(KBD_ANODE_6_PIN & KBD_ANODE_6_MASK)) state[6] |= 0x10;
	if (!(KBD_ANODE_7_PIN & KBD_ANODE_7_MASK)) state[7] |= 0x10;
	KBD_CATHODE_3_PORT |=  KBD_CATHODE_3_MASK;
	
	KBD_CATHODE_4_PORT &=~ KBD_CATHODE_4_MASK;
	_delay_us(KBD_PROP_DELAY);
	if (!(KBD_ANODE_0_PIN & KBD_ANODE_0_MASK)) state[0] |= 0x08;
	if (!(KBD_ANODE_1_PIN & KBD_ANODE_1_MASK)) state[1] |= 0x08;
	if (!(KBD_ANODE_2_PIN & KBD_ANODE_2_MASK)) state[2] |= 0x08;
	if (!(KBD_ANODE_3_PIN & KBD_ANODE_3_MASK)) state[3] |= 0x08;
	if (!(KBD_ANODE_4_PIN & KBD_ANODE_4_MASK)) state[4] |= 0x08;
	if (!(KBD_ANODE_5_PIN & KBD_ANODE_5_MASK)) state[5] |= 0x08;
	if (!(KBD_ANODE_6_PIN & KBD_ANODE_6_MASK)) state[6] |= 0x08;
	if (!(KBD_ANODE_7_PIN & KBD_ANODE_7_MASK)) state[7] |= 0x08;
	KBD_CATHODE_4_PORT |=  KBD_CATHODE_4_MASK;
	
	KBD_CATHODE_5_PORT &=~ KBD_CATHODE_5_MASK;
	_delay_us(KBD_PROP_DELAY);
	if (!(KBD_ANODE_0_PIN & KBD_ANODE_0_MASK)) state[0] |= 0x04;
	if (!(KBD_ANODE_1_PIN & KBD_ANODE_1_MASK)) state[1] |= 0x04;
	if (!(KBD_ANODE_2_PIN & KBD_ANODE_2_MASK)) state[2] |= 0x04;
	if (!(KBD_ANODE_3_PIN & KBD_ANODE_3_MASK)) state[3] |= 0x04;
	if (!(KBD_ANODE_4_PIN & KBD_ANODE_4_MASK)) state[4] |= 0x04;
	if (!(KBD_ANODE_5_PIN & KBD_ANODE_5_MASK)) state[5] |= 0x04;
	if (!(KBD_ANODE_6_PIN & KBD_ANODE_6_MASK)) state[6] |= 0x04;
	if (!(KBD_ANODE_7_PIN & KBD_ANODE_7_MASK)) state[7] |= 0x04;
	KBD_CATHODE_5_PORT |=  KBD_CATHODE_5_MASK;
	
	KBD_CATHODE_6_PORT &=~ KBD_CATHODE_6_MASK;
	_delay_us(KBD_PROP_DELAY);
	if (!(KBD_ANODE_0_PIN & KBD_ANODE_0_MASK)) state[0] |= 0x02;
	if (!(KBD_ANODE_1_PIN & KBD_ANODE_1_MASK)) state[1] |= 0x02;
	if (!(KBD_ANODE_2_PIN & KBD_ANODE_2_MASK)) state[2] |= 0x02;
	if (!(KBD_ANODE_3_PIN & KBD_ANODE_3_MASK)) state[3] |= 0x02;
	if (!(KBD_ANODE_4_PIN & KBD_ANODE_4_MASK)) state[4] |= 0x02;
	if (!(KBD_ANODE_5_PIN & KBD_ANODE_5_MASK)) state[5] |= 0x02;
	if (!(KBD_ANODE_6_PIN & KBD_ANODE_6_MASK)) state[6] |= 0x02;
	if (!(KBD_ANODE_7_PIN & KBD_ANODE_7_MASK)) state[7] |= 0x02;
	KBD_CATHODE_6_PORT |=  KBD_CATHODE_6_MASK;
	
	KBD_CATHODE_7_PORT &=~ KBD_CATHODE_7_MASK;
	_delay_us(KBD_PROP_DELAY);
	if (!(KBD_ANODE_0_PIN & KBD_ANODE_0_MASK)) state[0] |= 0x01;
	if (!(KBD_ANODE_1_PIN & KBD_ANODE_1_MASK)) state[1] |= 0x01;
	if (!(KBD_ANODE_2_PIN & KBD_ANODE_2_MASK)) state[2] |= 0x01;
	if (!(KBD_ANODE_3_PIN & KBD_ANODE_3_MASK)) state[3] |= 0x01;
	if (!(KBD_ANODE_4_PIN & KBD_ANODE_4_MASK)) state[4] |= 0x01;
	if (!(KBD_ANODE_5_PIN & KBD_ANODE_5_MASK)) state[5] |= 0x01;
	if (!(KBD_ANODE_6_PIN & KBD_ANODE_6_MASK)) state[6] |= 0x01;
	if (!(KBD_ANODE_7_PIN & KBD_ANODE_7_MASK)) state[7] |= 0x01;
	KBD_CATHODE_7_PORT |=  KBD_CATHODE_7_MASK;
}

static uint8_t lastState[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static uint8_t kbdBuffer[KBD_BUFFER_SIZE];
static int kbdBufStart = 0;
static int kbdBufEnd = 0;

static void writeKeyboardEvent(uint8_t evt) {
	kbdBuffer[kbdBufEnd] = evt; ++kbdBufEnd;
	if (kbdBufEnd >= KBD_BUFFER_SIZE) kbdBufEnd = 0;
	if (kbdBufEnd == kbdBufStart) {
		// Buffer overflowed. Ignore input.
		if (kbdBufEnd <= 0) kbdBufEnd = KBD_BUFFER_SIZE;
		--kbdBufEnd;
	}
}

uint8_t readKeyboardEvent() {
	uint8_t nextState[8], changed, key, row, p, r, col;
	readKeyboardState(nextState);
	for (changed = 0, key = 0, row = 0; row < 8; ++row) {
		changed |= (p = nextState[row] &~ lastState[row]);
		changed |= (r = lastState[row] &~ nextState[row]);
		lastState[row] = nextState[row];
		for (col = 0x80; col; col >>= 1, ++key) {
			if (p & col) writeKeyboardEvent(key | KBD_PRESSED );
			if (r & col) writeKeyboardEvent(key | KBD_RELEASED);
		}
	}
	if (changed) delay(KBD_DEBOUNCE_DELAY);
	if (kbdBufStart == kbdBufEnd) return 0;
	key = kbdBuffer[kbdBufStart]; ++kbdBufStart;
	if (kbdBufStart >= KBD_BUFFER_SIZE) kbdBufStart = 0;
	return key;
}
