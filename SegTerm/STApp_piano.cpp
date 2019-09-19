#include <Arduino.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include "KeyboardLib.h"
#include "PlayduinoLib.h"
#include "SegTerm_buttons.h"
#include "SegTerm_config.h"
#include "SegTerm_display.h"
#include "SegTerm_eeprom.h"
#include "STApp_piano.h"

static const uint8_t QWERTY_MAP[] PROGMEM = {
	44, 42, 0,  39, 37, 0,  0,  0,
	45, 43, 41, 40, 38, 36, 0,  0,
	0,  63, 61, 0,  58, 56, 0,  0,
	65, 64, 62, 60, 59, 57, 55, 0,
	67, 69, 71, 72, 0,  0,  0,  0,
	66, 68, 70, 0,  0,  0,  0,  0,
	47, 48, 50, 52, 53, 55, 57, 0,
	46, 0,  49, 51, 0,  54, 56, 0,
};

static uint8_t       pitch[POLYPHONY];
static unsigned int  period[POLYPHONY];
static unsigned int  dutyCycle[POLYPHONY];
static unsigned int  curPhase[POLYPHONY];
static uint8_t       pianoMode;

static void startPlaying(uint8_t p) {
	uint8_t i;
	for (i = 0; i < POLYPHONY; ++i) {
		if (pitch[i] == p) {
			return;
		}
	}
	for (i = 0; i < POLYPHONY; ++i) {
		if (!pitch[i]) {
			pitch[i] = p;
			period[i] = SAMPLE_RATE / getNoteFrequency(p);
			dutyCycle[i] = period[i] / DUTY_CYCLE_DIVISOR;
			curPhase[i] = 0;
			return;
		}
	}
}

static void stopPlaying(uint8_t p) {
	uint8_t i;
	for (i = 0; i < POLYPHONY; ++i) {
		if (pitch[i] == p) {
			pitch[i] = 0;
			period[i] = 0;
			dutyCycle[i] = 0;
			curPhase[i] = 0;
		}
	}
}

static void handleKeyboardEvent(uint8_t ch) {
	uint8_t p;
	if (pianoMode) {
		p = (ch & KBD_KEY) + 33;
	} else {
		if (!(p = pgm_read_byte(&QWERTY_MAP[ch & KBD_KEY]))) return;
		if (getKeyboardModifiers() & KBD_MOD_CAPS) p += 12;
	}
	if (ch & KBD_PRESSED) startPlaying(p);
	if (ch & KBD_RELEASED) stopPlaying(p);
}

static void setPianoMode(uint8_t mode) {
	uint8_t r = ((SEGTERM_ROWS - 1) >> 1);
	uint8_t c = (((SEGTERM_COLS << 2) - 12) >> 1);
	if ((pianoMode = mode)) {
		disableKeyboardModifiers();
		setKeyboardModifiers(0);
		setChar(r, c + 0, ' ');
		setChar(r, c + 1, 'P');
		setChar(r, c + 2, 'I');
		setChar(r, c + 3, 'A');
		setChar(r, c + 4, 'N');
		setChar(r, c + 5, 'O');
	} else {
		enableKeyboardModifiers();
		setChar(r, c + 0, 'Q');
		setChar(r, c + 1, 'W');
		setChar(r, c + 2, 'E');
		setChar(r, c + 3, 'R');
		setChar(r, c + 4, 'T');
		setChar(r, c + 5, 'Y');
	}
	setChar(r, c +  7, 'M');
	setChar(r, c +  8, 'O');
	setChar(r, c +  9, 'D');
	setChar(r, c + 10, 'E');
}

ISR(TIMER5_COMPA_vect) {
	uint8_t s = 0, i;
	for (i = 0; i < POLYPHONY; ++i) {
		if (pitch[i]) {
			if (curPhase[i] < dutyCycle[i]) s = 1;
			if (++curPhase[i] >= period[i]) curPhase[i] = 0;
		}
	}
	if (s) SPEAKER_PORT |=  SPEAKER_MASK;
	else   SPEAKER_PORT &=~ SPEAKER_MASK;
}

static void attachISR() {
	noInterrupts();
	TCCR5A = 0;
	TCCR5B = ((1 << WGM12) | (1 << CS10)); // CTC mode, no prescaling
	TCNT5  = 0;
	OCR5A  = (16000000 / SAMPLE_RATE);
	TIMSK5 = (1 << OCIE5A);
	interrupts();
}

static void detachISR() {
	noInterrupts();
	TCCR5A = 0;
	TCCR5B = 0;
	TCNT5  = 0;
	OCR5A  = 0;
	TIMSK5 = 0;
	interrupts();
}

bool piano_setup() {
	uint8_t i;
	pinMode(SPEAKER_PIN, OUTPUT);
	digitalWrite(SPEAKER_PIN, LOW);
	for (i = 0; i < POLYPHONY; ++i) {
		pitch[i] = 0;
		period[i] = 0;
		dutyCycle[i] = 0;
		curPhase[i] = 0;
	}
	clearRows(0, SEGTERM_ROWS);
	setPianoMode(EEPROM.read(EE_PIANOMODE));
	attachISR();
	return true;
}

bool piano_loop() {
	uint8_t ch;
	if ((ch = readKeyboardEvent())) {
		handleKeyboardEvent(ch);
	}
	if ((ch = getButtons())) {
		if (ch & (BUTTON_1 | BUTTON_3)) {
			setPianoMode(!pianoMode);
			EEPROMwrite(EE_PIANOMODE, pianoMode);
		}
		delay(KBD_DEBOUNCE_DELAY);
		while (getButtons() & ch);
		delay(KBD_DEBOUNCE_DELAY);
		if (ch & BUTTON_2) return false;
	}
	return true;
}

void piano_quit() {
	detachISR();
	enableKeyboardModifiers();
	clearRows(0, SEGTERM_ROWS);
}
