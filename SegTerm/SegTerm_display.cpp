#include <Arduino.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include "SegTerm_config.h"
#include "SegTerm_display.h"
#include "TinyI2C.h"

static const uint8_t FONT_A[] PROGMEM = {
	0x00, 0x0A, 0x22, 0x36, 0x2D, 0x24, 0x78, 0x42,
	0x39, 0x0F, 0x63, 0x46, 0x0C, 0x40, 0x08, 0x52,
	0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27,
	0x7F, 0x6F, 0x09, 0x0D, 0x46, 0x48, 0x70, 0x53,
	0x7B, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x3D,
	0x76, 0x06, 0x1E, 0x75, 0x38, 0x2B, 0x37, 0x3F,
	0x73, 0x67, 0x31, 0x6D, 0x07, 0x3E, 0x6A, 0x7E,
	0x49, 0x6E, 0x5B, 0x39, 0x64, 0x0F, 0x23, 0x08,
	0x60, 0x5F, 0x7C, 0x58, 0x5E, 0x7B, 0x71, 0x6F,
	0x74, 0x05, 0x0E, 0x75, 0x06, 0x55, 0x54, 0x5C,
	0x73, 0x67, 0x50, 0x6D, 0x78, 0x1C, 0x1D, 0x7E,
	0x48, 0x6E, 0x5B, 0x46, 0x30, 0x70, 0x01, 0x00,
};

static const uint8_t FONT_B[] PROGMEM = {
	0x00, 0x0A, 0x22, 0x36, 0x12, 0x24, 0x78, 0x42,
	0x58, 0x4C, 0x63, 0x46, 0x0C, 0x40, 0x10, 0x52,
	0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27,
	0x7F, 0x6F, 0x09, 0x0D, 0x61, 0x41, 0x43, 0x53,
	0x7B, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x3D,
	0x76, 0x05, 0x1E, 0x75, 0x38, 0x2B, 0x37, 0x6B,
	0x73, 0x67, 0x31, 0x2D, 0x07, 0x3E, 0x6A, 0x7E,
	0x49, 0x6E, 0x1B, 0x59, 0x64, 0x4D, 0x23, 0x08,
	0x60, 0x44, 0x7C, 0x58, 0x5E, 0x18, 0x33, 0x2F,
	0x74, 0x05, 0x0E, 0x75, 0x3C, 0x55, 0x54, 0x5C,
	0x73, 0x67, 0x50, 0x2D, 0x70, 0x1C, 0x1D, 0x7E,
	0x48, 0x6E, 0x1B, 0x69, 0x30, 0x4B, 0x01, 0x00,
};

static uint8_t font[96];
static uint8_t charBuf  [SEGTERM_ROWS][SEGTERM_COLS << 2];
static uint8_t chAttrBuf[SEGTERM_ROWS][SEGTERM_COLS << 2];
static uint8_t mdAttrBuf[SEGTERM_ROWS][SEGTERM_COLS     ];
static uint8_t lcMode;
static uint8_t dotMode;
static uint8_t brightness;
static uint8_t xorMask;
static uint8_t lockLevel;
static uint8_t blinkState;

static TinyI2C si[] = {
#if SEGTERM_ROWS >= 1
	TinyI2C(&DDRA, &PORTA, &PINA, (1<<0), &DDRA, &PORTA, &PINA, (1<<1)),
#endif
#if SEGTERM_ROWS >= 2
	TinyI2C(&DDRA, &PORTA, &PINA, (1<<2), &DDRA, &PORTA, &PINA, (1<<3)),
#endif
#if SEGTERM_ROWS >= 3
	TinyI2C(&DDRA, &PORTA, &PINA, (1<<4), &DDRA, &PORTA, &PINA, (1<<5)),
#endif
#if SEGTERM_ROWS >= 4
	TinyI2C(&DDRA, &PORTA, &PINA, (1<<6), &DDRA, &PORTA, &PINA, (1<<7)),
#endif
#if SEGTERM_ROWS >= 5
	TinyI2C(&DDRC, &PORTC, &PINC, (1<<7), &DDRC, &PORTC, &PINC, (1<<6)),
#endif
#if SEGTERM_ROWS >= 6
	TinyI2C(&DDRC, &PORTC, &PINC, (1<<5), &DDRC, &PORTC, &PINC, (1<<4)),
#endif
#if SEGTERM_ROWS >= 7
	TinyI2C(&DDRC, &PORTC, &PINC, (1<<3), &DDRC, &PORTC, &PINC, (1<<2)),
#endif
#if SEGTERM_ROWS >= 8
	TinyI2C(&DDRC, &PORTC, &PINC, (1<<1), &DDRC, &PORTC, &PINC, (1<<0)),
#endif
#if SEGTERM_ROWS >= 9
	TinyI2C(&DDRD, &PORTD, &PIND, (1<<7), &DDRG, &PORTG, &PING, (1<<2)),
#endif
#if SEGTERM_ROWS >= 10
	TinyI2C(&DDRG, &PORTG, &PING, (1<<1), &DDRG, &PORTG, &PING, (1<<0)),
#endif
#if SEGTERM_ROWS >= 11
	TinyI2C(&DDRL, &PORTL, &PINL, (1<<7), &DDRL, &PORTL, &PINL, (1<<6)),
#endif
#if SEGTERM_ROWS >= 12
	TinyI2C(&DDRL, &PORTL, &PINL, (1<<5), &DDRL, &PORTL, &PINL, (1<<4)),
#endif
#if SEGTERM_ROWS >= 13
	TinyI2C(&DDRL, &PORTL, &PINL, (1<<3), &DDRL, &PORTL, &PINL, (1<<2)),
#endif
#if SEGTERM_ROWS >= 14
	TinyI2C(&DDRL, &PORTL, &PINL, (1<<1), &DDRL, &PORTL, &PINL, (1<<0)),
#endif
#if SEGTERM_ROWS >= 15
	TinyI2C(&DDRB, &PORTB, &PINB, (1<<3), &DDRB, &PORTB, &PINB, (1<<2)),
#endif
#if SEGTERM_ROWS >= 16
	TinyI2C(&DDRB, &PORTB, &PINB, (1<<1), &DDRB, &PORTB, &PINB, (1<<0)),
#endif
};

void initDisplay() {
	uint8_t i, row, col, addr;

	for (i = 0; i < 96; ++i) {
		font[i] = pgm_read_byte(&FONT_A[i]);
	}
	for (row = 0; row < SEGTERM_ROWS; ++row) {
		for (col = 0; col < (SEGTERM_COLS << 2); ++col) {
			charBuf  [row][col] = 0;
			chAttrBuf[row][col] = 0;
		}
		for (col = 0; col < SEGTERM_COLS; ++col) {
			mdAttrBuf[row][col] = 0xF0;
		}
	}
	lcMode = 1;
	dotMode = 0;
	brightness = 15;
	xorMask = SEGTERM_XORMASK_DEFAULT;
	lockLevel = 0;
	blinkState = (millis() / SEGTERM_BLINK_RATE) & 1;

	for (row = 0; row < SEGTERM_ROWS; ++row) {
		si[row].init();
		for (col = 0; col < SEGTERM_COLS; ++col) {
			addr = HT16K33_BASE_ADDRESS | (col << 1);
			si[row].start(addr);
			si[row].write(HT16K33_START_OSCILLATOR);
			si[row].restart(addr);
			si[row].write(0);
			si[row].write(0); si[row].write(0);
			si[row].write(0); si[row].write(0);
			si[row].write(0); si[row].write(0);
			si[row].write(0); si[row].write(0);
			si[row].write(0); si[row].write(0);
			si[row].restart(addr);
			si[row].write(HT16K33_CMD_BRIGHTNESS | 15);
			si[row].restart(addr);
			si[row].write(HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON);
			si[row].stop();
		}
	}
}

/***************************** UTILITY FUNCTIONS *****************************/

static uint8_t getRawChar(uint8_t row, uint8_t col) {
	uint8_t attr, data;

	attr = chAttrBuf[row][col];
	if (
		(attr & SEGTERM_CHATTR_HIDDEN) ||
		((attr & SEGTERM_CHATTR_BLINK) && blinkState)
	) {
		data = 0;
	} else {
		data = charBuf[row][col];
		if (!(attr & SEGTERM_CHATTR_RAW)) {
			uint8_t high, isuc, islc;
			high = data & 0x80; data &= 0x7F;
			isuc = (data >= 'A' && data <= 'Z');
			islc = (data >= 'a' && data <= 'z');
			if (islc && !lcMode) data -= 32;
			if (data < 32) data = 0;
			else data = font[data - 32];
			if ((dotMode & 0x80) && high) data ^= 0x80;
			if ((dotMode & 0x02) && isuc) data ^= 0x80;
			if ((dotMode & 0x01) && islc) data ^= 0x80;
		}
	}

	if (attr & SEGTERM_CHATTR_INVERT ) data ^= 0x7F;
	if (attr & SEGTERM_CHATTR_XORMASK) data ^= xorMask;
	if (attr & SEGTERM_CHATTR_DOT    ) data ^= 0x80;
	return data;
}

static void flushChar(uint8_t row, uint8_t col) {
	uint8_t chcol, addr, reg, data;
	chcol = col; col >>= 2;
	addr = HT16K33_BASE_ADDRESS | (col << 1);
	reg = chcol & 3; if (reg >= 2) ++reg; reg <<= 1;
	data = getRawChar(row, chcol);
	si[row].start(addr);
	si[row].write(reg);
	si[row].write(data);
	si[row].write(0);
	si[row].stop();
}

static void flushRow(uint8_t row) {
	uint8_t col, chcol, d0, d1, d2, d3, attr, br, bl, co, addr;
	for (col = 0; col < SEGTERM_COLS; ++col) {
		chcol = col << 2;
		d0 = getRawChar(row, chcol); ++chcol;
		d1 = getRawChar(row, chcol); ++chcol;
		d2 = getRawChar(row, chcol); ++chcol;
		d3 = getRawChar(row, chcol);
		attr = mdAttrBuf[row][col];
		br = (((attr >> 4) & 15) * brightness) / 15;
		bl = ((attr >> 1) & 6);
		co = (attr & 3);
		addr = HT16K33_BASE_ADDRESS | (col << 1);
		si[row].start(addr);
		si[row].write(0);
		si[row].write(d0); si[row].write(0);
		si[row].write(d1); si[row].write(0);
		si[row].write(co); si[row].write(0);
		si[row].write(d2); si[row].write(0);
		si[row].write(d3); si[row].write(0);
		si[row].restart(addr);
		si[row].write(HT16K33_CMD_BRIGHTNESS | br);
		si[row].restart(addr);
		si[row].write(HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | bl);
		si[row].stop();
	}
}

static void flushRows(uint8_t start, uint8_t end) {
	while (start < end) { flushRow(start); ++start; }
}

static void flushScreen() {
	flushRows(0, SEGTERM_ROWS);
}

static void EEPROMwrite(const int addr, const uint8_t val) {
	if (EEPROM.read(addr) != val) {
		EEPROM.write(addr, val);
	}
}

/******************************** PUBLIC API ********************************/

void loadFont(uint8_t fn) {
	uint8_t i = 0;
	switch (fn) {
		case 0: while(i<96){ font[i]=pgm_read_byte(&FONT_A[i]); ++i; } break;
		case 1: while(i<96){ font[i]=pgm_read_byte(&FONT_B[i]); ++i; } break;
	}
	if (!lockLevel) flushScreen();
}

void loadFontFromEEPROM(int addr) {
	for (uint8_t i = 0; i < 96; ++i, ++addr) {
		font[i] = EEPROM.read(addr);
	}
	if (!lockLevel) flushScreen();
}

void saveFontToEEPROM(int addr) {
	for (uint8_t i = 0; i < 96; ++i, ++addr) {
		EEPROMwrite(addr, font[i]);
	}
}

void loadDefaultDisplaySettings() {
	lcMode = 1;
	dotMode = 0;
	brightness = 15;
	xorMask = SEGTERM_XORMASK_DEFAULT;
	loadFont(0);
	if (!lockLevel) flushScreen();
}

void loadDisplaySettingsFromEEPROM(int addr) {
	if (EEPROM.read(addr++) == 0x57) {
		if (EEPROM.read(addr++) == 0xEE) {
			lcMode = EEPROM.read(addr++);
			dotMode = EEPROM.read(addr++);
			brightness = EEPROM.read(addr++);
			xorMask = EEPROM.read(addr++);
			loadFontFromEEPROM(addr + 10);
			if (!lockLevel) flushScreen();
			return;
		}
	}
	loadDefaultDisplaySettings();
}

void saveDisplaySettingsToEEPROM(int addr) {
	EEPROMwrite(addr++, 0x57);
	EEPROMwrite(addr++, 0xEE);
	EEPROMwrite(addr++, lcMode);
	EEPROMwrite(addr++, dotMode);
	EEPROMwrite(addr++, brightness);
	EEPROMwrite(addr++, xorMask);
	saveFontToEEPROM(addr + 10);
}

uint8_t getFontChar(uint8_t ch) {
	ch &= 0x7F;
	if (ch < 0x20) return 0;
	return font[ch - 32];
}

void setFontChar(uint8_t ch, uint8_t data) {
	ch &= 0x7F;
	if (ch < 0x20) return;
	font[ch - 32] = data;
	if (!lockLevel) flushScreen();
}

uint8_t getChar(uint8_t row, uint8_t col) {
	return charBuf[row][col];
}

void setChar(uint8_t row, uint8_t col, uint8_t ch) {
	charBuf[row][col] = ch;
	if (!lockLevel) flushChar(row, col);
}

uint8_t getChAttr(uint8_t row, uint8_t col) {
	return chAttrBuf[row][col];
}

void setChAttr(uint8_t row, uint8_t col, uint8_t a) {
	chAttrBuf[row][col] = a;
	if (!lockLevel) flushChar(row, col);
}

void setCharAndAttr(uint8_t row, uint8_t col, uint8_t ch, uint8_t a) {
	charBuf  [row][col] = ch;
	chAttrBuf[row][col] = a;
	if (!lockLevel) flushChar(row, col);
}

uint8_t getMdAttr(uint8_t row, uint8_t col) {
	return mdAttrBuf[row][col];
}

void setMdAttr(uint8_t row, uint8_t col, uint8_t a) {
	mdAttrBuf[row][col] = a;
	if (!lockLevel) {
		uint8_t addr = HT16K33_BASE_ADDRESS | (col << 1);
		uint8_t br = (((a >> 4) & 15) * brightness) / 15;
		uint8_t bl = ((a >> 1) & 6);
		si[row].start(addr);
		si[row].write(4);
		si[row].write(a & 3);
		si[row].write(0);
		si[row].restart(addr);
		si[row].write(HT16K33_CMD_BRIGHTNESS | br);
		si[row].restart(addr);
		si[row].write(HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | bl);
		si[row].stop();
	}
}

uint8_t getMdBright(uint8_t row, uint8_t col) {
	return (mdAttrBuf[row][col] >> 4) & 15;
}

void setMdBright(uint8_t row, uint8_t col, uint8_t b) {
	mdAttrBuf[row][col] &= 0x0F;
	mdAttrBuf[row][col] |= (b << 4);
	if (!lockLevel) {
		uint8_t addr = HT16K33_BASE_ADDRESS | (col << 1);
		uint8_t br = ((b & 15) * brightness) / 15;
		si[row].start(addr);
		si[row].write(HT16K33_CMD_BRIGHTNESS | br);
		si[row].stop();
	}
}

uint8_t getMdBlink(uint8_t row, uint8_t col) {
	return (mdAttrBuf[row][col] >> 2) & 3;
}

void setMdBlink(uint8_t row, uint8_t col, uint8_t b) {
	mdAttrBuf[row][col] &=~ (3 << 2);
	mdAttrBuf[row][col] |= ((b & 3) << 2);
	if (!lockLevel) {
		uint8_t addr = HT16K33_BASE_ADDRESS | (col << 1);
		uint8_t bl = ((b & 3) << 1);
		si[row].start(addr);
		si[row].write(HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | bl);
		si[row].stop();
	}
}

uint8_t getMdColon(uint8_t row, uint8_t col) {
	return mdAttrBuf[row][col] & 3;
}

void setMdColon(uint8_t row, uint8_t col, uint8_t c) {
	mdAttrBuf[row][col] &=~ 3;
	mdAttrBuf[row][col] |= (c & 3);
	if (!lockLevel) {
		uint8_t addr = HT16K33_BASE_ADDRESS | (col << 1);
		si[row].start(addr);
		si[row].write(4);
		si[row].write(c & 3);
		si[row].write(0);
		si[row].stop();
	}
}

uint8_t getLcMode() {
	return lcMode;
}

void setLcMode(uint8_t lc) {
	lcMode = lc;
	if (!lockLevel) flushScreen();
}

uint8_t getDotMode() {
	return dotMode;
}

void setDotMode(uint8_t d) {
	dotMode = d;
	if (!lockLevel) flushScreen();
}

uint8_t getBrightness() {
	return brightness;
}

void setBrightness(uint8_t b) {
	brightness = b & 15;
	if (!lockLevel) {
		for (uint8_t row = 0; row < SEGTERM_ROWS; ++row) {
			for (uint8_t col = 0; col < SEGTERM_COLS; ++col) {
				uint8_t addr = HT16K33_BASE_ADDRESS | (col << 1);
				uint8_t attr = mdAttrBuf[row][col];
				uint8_t br = (((attr >> 4) & 15) * brightness) / 15;
				si[row].start(addr);
				si[row].write(HT16K33_CMD_BRIGHTNESS | br);
				si[row].stop();
			}
		}
	}
}

uint8_t getXorMask() {
	return xorMask;
}

void setXorMask(uint8_t m) {
	xorMask = m;
	if (!lockLevel) flushScreen();
}

uint8_t lockScreen() {
	++lockLevel;
	return lockLevel;
}

uint8_t unlockScreen() {
	if (lockLevel) --lockLevel;
	if (!lockLevel) flushScreen();
	return lockLevel;
}

void displayIdle() {
	uint8_t newState, row, col, attr;
	if (!lockLevel) {
		newState = (millis() / SEGTERM_BLINK_RATE) & 1;
		if (blinkState != newState) {
			blinkState = newState;
			for (row = 0; row < SEGTERM_ROWS; ++row) {
				for (col = 0; col < (SEGTERM_COLS << 2); ++col) {
					attr = chAttrBuf[row][col];
					if ((attr & SEGTERM_CHATTR_BLINK) && !(attr & SEGTERM_CHATTR_HIDDEN)) {
						flushChar(row, col);
					}
				}
			}
		}
	}
}

void scrollRows(uint8_t top, uint8_t bottom, int8_t dir) {
	uint8_t row; uint8_t col;
	while (dir < 0) {
		for (row = top; row < bottom - 1; ++row) {
			for (col = 0; col < (SEGTERM_COLS << 2); ++col) {
				charBuf  [row][col] = charBuf  [row + 1][col];
				chAttrBuf[row][col] = chAttrBuf[row + 1][col];
			}
			for (col = 0; col < SEGTERM_COLS; ++col) {
				mdAttrBuf[row][col] = mdAttrBuf[row + 1][col];
			}
		}
		for (col = 0; col < (SEGTERM_COLS << 2); ++col) {
			charBuf[bottom - 1][col] = 0;
		}
		++dir;
	}
	while (dir > 0) {
		for (row = bottom - 1; row > top; --row) {
			for (col = 0; col < (SEGTERM_COLS << 2); ++col) {
				charBuf  [row][col] = charBuf  [row - 1][col];
				chAttrBuf[row][col] = chAttrBuf[row - 1][col];
			}
			for (col = 0; col < SEGTERM_COLS; ++col) {
				mdAttrBuf[row][col] = mdAttrBuf[row - 1][col];
			}
		}
		for (col = 0; col < (SEGTERM_COLS << 2); ++col) {
			charBuf[top][col] = 0;
		}
		--dir;
	}
	if (!lockLevel) flushRows(top, bottom);
}

void scrollCols(uint8_t row, uint8_t left, uint8_t right, int8_t dir) {
	uint8_t col;
	++lockLevel;
	while (dir < 0) {
		for (col = left; col < right - 1; ++col) {
			charBuf  [row][col] = charBuf  [row][col + 1];
			chAttrBuf[row][col] = chAttrBuf[row][col + 1];
			setMdAttr(row, (col >> 2), getMdAttr(row, ((col + 1) >> 2)));
		}
		charBuf[row][right - 1] = 0;
		++dir;
	}
	while (dir > 0) {
		for (col = right - 1; col > left; --col) {
			charBuf  [row][col] = charBuf  [row][col - 1];
			chAttrBuf[row][col] = chAttrBuf[row][col - 1];
			setMdAttr(row, (col >> 2), getMdAttr(row, ((col - 1) >> 2)));
		}
		charBuf[row][left] = 0;
		--dir;
	}
	--lockLevel;
	if (!lockLevel) flushRow(row);
}

void clearRows(uint8_t row1, uint8_t row2) {
	uint8_t row; uint8_t col;
	for (row = row1; row < row2; ++row) {
		for (col = 0; col < (SEGTERM_COLS << 2); ++col) {
			charBuf  [row][col] = 0;
			chAttrBuf[row][col] = 0;
		}
		for (col = 0; col < SEGTERM_COLS; ++col) {
			mdAttrBuf[row][col] = 0xF0;
		}
	}
	if (!lockLevel) flushRows(row1, row2);
}

void clearCols(uint8_t row, uint8_t col1, uint8_t col2) {
	uint8_t col;
	++lockLevel;
	for (col = col1; col < col2; ++col) {
		charBuf  [row][col] = 0;
		chAttrBuf[row][col] = 0;
		setMdAttr(row, (col >> 2), 0xF0);
	}
	--lockLevel;
	if (!lockLevel) flushRow(row);
}

void fillScreen(uint8_t ch, uint8_t chattr, uint8_t mdattr) {
	uint8_t row, col;
	for (row = 0; row < SEGTERM_ROWS; ++row) {
		for (col = 0; col < (SEGTERM_COLS << 2); ++col) {
			charBuf  [row][col] = ch;
			chAttrBuf[row][col] = chattr;
		}
		for (col = 0; col < SEGTERM_COLS; ++col) {
			mdAttrBuf[row][col] = mdattr;
		}
	}
	if (!lockLevel) flushScreen();
}
