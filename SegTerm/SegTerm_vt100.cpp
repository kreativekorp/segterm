#include <Arduino.h>
#include <inttypes.h>
#include "SegTerm_config.h"
#include "SegTerm_display.h"
#include "SegTerm_vt100.h"

static uint8_t vtMode;
static uint8_t vtState;
static uint8_t vtParam[VT100_MAX_PARAMS];
static uint8_t vtParamPtr;
static uint8_t vtEscVariant;
static uint8_t vtOutBuf[VT100_OUTBUF_SIZE];
static uint8_t vtOutBufStart;
static uint8_t vtOutBufEnd;
static uint8_t tabStops[SEGTERM_COLS];
static uint8_t scrollTop;
static uint8_t scrollBottom;
static uint8_t cursorY;
static uint8_t cursorX;
static uint8_t txtAttr;
static uint8_t txtColor;
static uint8_t saveCursorY;
static uint8_t saveCursorX;
static uint8_t saveTxtAttr;
static uint8_t saveTxtColor;
static unsigned long lastBlinkTime;

void initVT100() {
	uint8_t i;
	vtMode = VT100_MODE_DEFAULT;
	vtState = VT100_STATE_BASE;
	vtOutBufStart = 0;
	vtOutBufEnd = 0;
	for (i = 0; i < SEGTERM_COLS; ++i) tabStops[i] = 0;
	scrollTop = 0;
	scrollBottom = SEGTERM_ROWS;
	cursorY = 0;
	cursorX = 0;
	txtAttr = 0;
	txtColor = 7;
	saveCursorY = 0;
	saveCursorX = 0;
	saveTxtAttr = 0;
	saveTxtColor = 7;
	lastBlinkTime = 0;

#ifdef SEGTERM_KBDLED_1
	pinMode(SEGTERM_KBDLED_1, OUTPUT);
	digitalWrite(SEGTERM_KBDLED_1, LOW);
#endif
#ifdef SEGTERM_KBDLED_2
	pinMode(SEGTERM_KBDLED_2, OUTPUT);
	digitalWrite(SEGTERM_KBDLED_2, LOW);
#endif
#ifdef SEGTERM_KBDLED_3
	pinMode(SEGTERM_KBDLED_3, OUTPUT);
	digitalWrite(SEGTERM_KBDLED_3, LOW);
#endif
#ifdef SEGTERM_KBDLED_4
	pinMode(SEGTERM_KBDLED_4, OUTPUT);
	digitalWrite(SEGTERM_KBDLED_4, LOW);
#endif
}

/***************************** UTILITY FUNCTIONS *****************************/

static void vtTab() {
	if (cursorX < (SEGTERM_COLS << 2)) {
		do ++cursorX;
		while (
			(cursorX < (SEGTERM_COLS << 2)) &&
			!(tabStops[cursorX >> 2] & (1 << (cursorX & 3)))
		);
	}
}

static void vtReverseTab() {
	if (cursorX > 0) {
		do --cursorX;
		while (
			(cursorX > 0) &&
			!(tabStops[cursorX >> 2] & (1 << (cursorX & 3)))
		);
	}
}

static void vtIndex() {
	if (cursorY == (scrollBottom - 1)) {
		scrollRows(scrollTop, scrollBottom, -1);
	} else if (cursorY < (SEGTERM_ROWS - 1)) {
		++cursorY;
	}
}

static void vtReverseIndex() {
	if (cursorY == scrollTop) {
		scrollRows(scrollTop, scrollBottom, +1);
	} else if (cursorY > 0) {
		--cursorY;
	}
}

static void vtPrintChar(uint8_t raw, uint8_t ch) {
	uint8_t chattr = 0;
	uint8_t mdattr = 0;
	
	if (cursorX >= (SEGTERM_COLS << 2)) {
		cursorX = 0;
		vtIndex();
	}
	if (vtMode & VT100_MODE_INSERT) {
		scrollCols(cursorY, cursorX, (SEGTERM_COLS << 2), +1);
	}
	
	if (txtAttr & VT100_ATTR_HIDDEN   ) chattr |= SEGTERM_CHATTR_HIDDEN;
	if (txtAttr & VT100_ATTR_BLINK    ) chattr |= SEGTERM_CHATTR_BLINK;
	if (raw                           ) chattr |= SEGTERM_CHATTR_RAW;
	if (txtAttr & VT100_ATTR_REVERSE  ) chattr |= SEGTERM_CHATTR_INVERT;
	if (txtAttr & VT100_ATTR_UNDERLINE) chattr |= SEGTERM_CHATTR_DOT;
	
	if (txtColor & VT100_COLOR_RED  ) mdattr += VT100_BRIGHTNESS_RED  ;
	if (txtColor & VT100_COLOR_GREEN) mdattr += VT100_BRIGHTNESS_GREEN;
	if (txtColor & VT100_COLOR_BLUE ) mdattr += VT100_BRIGHTNESS_BLUE ;
	
	if ((txtColor & VT100_COLOR_BRIGHT) || (txtAttr & VT100_ATTR_BRIGHT)) {
		if (txtAttr & VT100_ATTR_DIM) mdattr *= VT100_BRIGHTNESS_NORMAL;
		else                          mdattr *= VT100_BRIGHTNESS_BRIGHT;
	} else {
		if (txtAttr & VT100_ATTR_DIM) mdattr *= VT100_BRIGHTNESS_DIM   ;
		else                          mdattr *= VT100_BRIGHTNESS_NORMAL;
	}
	
	mdattr /= 15;
	mdattr <<= 4;
	
	setCharAndAttr(cursorY, cursorX, ch, chattr);
	setMdAttr(cursorY, (cursorX >> 2), mdattr);
	
	++cursorX;
}

static void vtOutput(uint8_t ch) {
	vtOutBuf[vtOutBufEnd] = ch;
	++vtOutBufEnd;
	if (vtOutBufEnd >= VT100_OUTBUF_SIZE) {
		vtOutBufEnd = 0;
	}
	if (vtOutBufStart == vtOutBufEnd) {
		++vtOutBufStart;
		if (vtOutBufStart >= VT100_OUTBUF_SIZE) {
			vtOutBufStart = 0;
		}
	}
}

static void vtOutputNum(uint8_t v) {
	uint8_t u = v % 10; v /= 10;
	if (v) {
		uint8_t x = v % 10; v /= 10;
		if (v) {
			vtOutput('0' + v);
		}
		vtOutput('0' + x);
	}
	vtOutput('0' + u);
}

static void vtOutputNum16(uint16_t v) {
	uint8_t u = v % 10; v /= 10;
	if (v) {
		uint8_t x = v % 10; v /= 10;
		if (v) {
			uint8_t h = v % 10; v /= 10;
			if (v) {
				uint8_t t = v % 10; v /= 10;
				if (v) {
					vtOutput('0' + v);
				}
				vtOutput('0' + t);
			}
			vtOutput('0' + h);
		}
		vtOutput('0' + x);
	}
	vtOutput('0' + u);
}

static int freeRAM() {
  extern int __heap_start, * __brkval; int v;
  return (int)&v - ((__brkval == 0) ? (int)&__heap_start : (int)__brkval);
}

/***************************** INPUT PROCESSING *****************************/

static void vtInputControl(uint8_t ch) {
	uint8_t i;
	switch (ch) {

		// NUL - ignored
		case 0x00:
			return;

		// ETX - half-duplex turnaround (not implemented)
		// EOT - disconnect (not implemented)
		// ENQ - answerback message (not implemented)

		// BEL - terminal bell
		case 0x07:
			tone(SEGTERM_BELL_PIN, SEGTERM_BELL_FRQ, SEGTERM_BELL_DUR);
			break;

		// BS - backspace
		case 0x08:
			if (cursorX > 0) --cursorX;
			break;

		// HT - horizontal tab
		case 0x09:
			vtTab();
			break;

		// LF - linefeed or newline
		// VT - processed as LF
		// FF - processed as LF
		case 0x0A: case 0x0B: case 0x0C:
			if (vtMode & VT100_MODE_NEWLINE) cursorX = 0;
			vtIndex();
			break;

		// CR - carriage return
		case 0x0D:
			cursorX = 0;
			break;

		// SO - switch to G1 character set (not implemented)
		// SI - switch to G0 character set (not implemented)
		// DC1 - XON - continue transmitting (not implemented)
		// DC3 - XOFF - stop transmitting (not implemented)
		// DC4 - SSU session management command (VT420; not implemented)

		// CAN - cancel control sequence (VT220; same as SUB on VT102)
		case 0x18:
			break;

		// SUB - cancel control sequence, print substitution character
		case 0x1A:
			vtPrintChar(1, 0x65);
			break;

		// ESC - escape sequence introducer
		case 0x1B:
			vtState = VT100_STATE_ESC;
			for (i = 0; i < VT100_MAX_PARAMS; i++) vtParam[i] = 0;
			vtParamPtr = 0;
			vtEscVariant = 0;
			return;

	}
	vtState = VT100_STATE_BASE;
}

static void vtInputEscapeVT52(uint8_t ch) {
	switch (ch) {

		// ESC < - ANSI mode
		case '<':
			vtMode &=~ VT100_MODE_VT52;
			break;

		// ESC = - alternate keypad mode (not implemented)
		// ESC > - numeric keypad mode (not implemented)

		// ESC A - cursor up
		case 'A':
			if (cursorY > 0) --cursorY;
			break;

		// ESC B - cursor down
		case 'B':
			if (cursorY < (SEGTERM_ROWS-1)) ++cursorY;
			break;

		// ESC C - cursor right
		case 'C':
			if (cursorX < (SEGTERM_COLS<<2)) ++cursorX;
			break;

		// ESC D - cursor left
		case 'D':
			if (cursorX > 0) --cursorX;
			break;

		// ESC F - graphics character set (not implemented)
		// ESC G - ASCII character set (not implemented)

		// ESC H - cursor home
		case 'H':
			cursorX = cursorY = 0;
			break;

		// ESC I - reverse line feed
		case 'I':
			vtReverseIndex();
			break;

		// ESC J - erase to end of screen
		case 'J':
			clearRows(cursorY + 1, SEGTERM_ROWS);
			// fallthrough

		// ESC K - erase to end of line
		case 'K':
			clearCols(cursorY, cursorX, (SEGTERM_COLS<<2));
			break;

		// ESC L - insert line (listed on Wikipedia but not actually a VT52 sequence)
		case 'L':
			if (cursorY >= scrollTop && cursorY < scrollBottom)
				scrollRows(cursorY, scrollBottom, +1);
			break;

		// ESC M - delete line (listed on Wikipedia but not actually a VT52 sequence)
		case 'M':
			if (cursorY >= scrollTop && cursorY < scrollBottom)
				scrollRows(cursorY, scrollBottom, -1);
			break;

		// ESC V - print cursor line (not implemented)
		// ESC W - enter printer controller mode (not implemented)
		// ESC X - exit printer controller mode (not implemented)

		// ESC Y l c - direct cursor address
		case 'Y':
			vtState = VT100_STATE_VT52_DCA;
			return;

		// ESC Z - identify
		case 'Z':
			vtOutput(0x1B);
			vtOutput('/');
			vtOutput('Z');
			break;

		// ESC ] - print screen (not implemented)
		// ESC ^ - enter auto print mode (not implemented)
		// ESC _ - exit auto print mode (not implemented)

	}
	vtState = VT100_STATE_BASE;
}

static void vtInputCursorAddressVT52(uint8_t ch) {
	if (vtParamPtr) {
		cursorX = ((ch & 0x7F) - ' ');
		if (cursorX > (SEGTERM_COLS<<2)) cursorX = (SEGTERM_COLS<<2);
		vtState = VT100_STATE_BASE;
	} else {
		cursorY = ((ch & 0x7F) - ' ');
		if (cursorY > (SEGTERM_ROWS-1)) cursorY = (SEGTERM_ROWS-1);
		++vtParamPtr;
	}
}

static void vtInputEscapeBasic(uint8_t ch) {
	switch (ch) {

		// ESC 6 - backward index (VT420; not implemented)

		// ESC 7 - save cursor and attributes
		case '7':
			saveCursorY = cursorY;
			saveCursorX = cursorX;
			saveTxtAttr = txtAttr;
			saveTxtColor = txtColor;
			break;

		// ESC 8 - restore cursor and attributes
		case '8':
			cursorY = saveCursorY;
			cursorX = saveCursorX;
			txtAttr = saveTxtAttr;
			txtColor = saveTxtColor;
			break;

		// ESC 9 - forward index (VT420; not implemented)

		// ESC < - VT52 mode
		case '<':
			vtMode |= VT100_MODE_VT52;
			break;

		// ESC = - alternate keypad mode (not implemented)
		// ESC > - numeric keypad mode (not implemented)

		// ESC D - index
		case 'D':
			vtIndex();
			break;

		// ESC E - next line
		case 'E':
			cursorX = 0;
			vtIndex();
			break;

		// ESC H - horizontal tab set
		case 'H':
			tabStops[cursorX >> 2] |= (1 << (cursorX & 3));
			break;

		// ESC M - reverse index
		case 'M':
			vtReverseIndex();
			break;

		// ESC N - single shift 2 (not implemented)
		// ESC O - single shift 3 (not implemented)

		// ESC P - device control string
		case 'P':
			vtState = VT100_STATE_DCS;
			return;

		// ESC Q - define function key (SCO; not implemented)

		// ESC X - start of string
		case 'X':
			vtState = VT100_STATE_SOS;
			return;

		// ESC Z - identify
		case 'Z':
			vtOutput(0x1B);
			vtOutput('[');
			vtOutput('?');
			vtOutput('6');
			vtOutput('c');
			break;

		// ESC [ - control sequence introducer
		case '[':
			vtState = VT100_STATE_CSI;
			return;

		// ESC \ - string terminator
		case '\\':
			break;

		// ESC ] - operating system command
		case ']':
			vtState = VT100_STATE_OSC;
			return;

		// ESC ^ - privacy message
		case '^':
			vtState = VT100_STATE_PM;
			return;

		// ESC _ - application program command
		case '_':
			vtState = VT100_STATE_APC;
			return;

		// ESC c - reset to initial state
		case 'c':
			initVT100();
			break;

		// ESC n - lock shift G2 (VT220; not implemented)
		// ESC o - lock shift G3 (VT220; not implemented)

		// ESC | - lock shift G3 right (VT220; not implemented)
		// ESC } - lock shift G2 right (VT220; not implemented)
		// ESC ~ - lock shift G1 right (VT220; not implemented)

	}
	vtState = VT100_STATE_BASE;
}

static void vtInputEscapeHash(uint8_t ch) {
	switch (ch) {

		// ESC # 3 - double-height, top half (not implemented)
		// ESC # 4 - double-height, bottom half (not implemented)
		// ESC # 5 - single-width, single-height (not implemented)
		// ESC # 6 - double-width, single-height (not implemented)

		// ESC # 8 - screen alignment test
		case '8':
			fillScreen(0x79, SEGTERM_CHATTR_RAW, (VT100_BRIGHTNESS_NORMAL<<4));
			break;

		// ESC # a-i - LED segment test (SegTerm extension)
		case 'a': fillScreen(0x01, SEGTERM_CHATTR_RAW, (VT100_BRIGHTNESS_NORMAL<<4)); break;
		case 'b': fillScreen(0x02, SEGTERM_CHATTR_RAW, (VT100_BRIGHTNESS_NORMAL<<4)); break;
		case 'c': fillScreen(0x04, SEGTERM_CHATTR_RAW, (VT100_BRIGHTNESS_NORMAL<<4)); break;
		case 'd': fillScreen(0x08, SEGTERM_CHATTR_RAW, (VT100_BRIGHTNESS_NORMAL<<4)); break;
		case 'e': fillScreen(0x10, SEGTERM_CHATTR_RAW, (VT100_BRIGHTNESS_NORMAL<<4)); break;
		case 'f': fillScreen(0x20, SEGTERM_CHATTR_RAW, (VT100_BRIGHTNESS_NORMAL<<4)); break;
		case 'g': fillScreen(0x40, SEGTERM_CHATTR_RAW, (VT100_BRIGHTNESS_NORMAL<<4)); break;
		case 'h': fillScreen(0x80, SEGTERM_CHATTR_RAW, (VT100_BRIGHTNESS_NORMAL<<4)); break;
		case 'i': fillScreen(0, 0, ((VT100_BRIGHTNESS_NORMAL<<4)|3)); break;

		// ESC # y - LED segment test (SegTerm extension)
		case 'y':
			fillScreen(0xFF, SEGTERM_CHATTR_RAW, ((VT100_BRIGHTNESS_NORMAL<<4)|3));
			break;

		// ESC # z - clear screen (SegTerm extension)
		case 'z':
			fillScreen(0, 0, (VT100_BRIGHTNESS_NORMAL<<4));
			break;

	}
	vtState = VT100_STATE_BASE;
}

static void vtInputEscapeFinish(uint8_t ch) {
	switch (vtEscVariant) {

		case 0:
			vtInputEscapeBasic(ch);
			return;

		// case ' ':
			// ESC SP F - 7-bit control transmission (VT220)
			// ESC SP G - 8-bit control transmission (VT220)
			// ESC SP L - ANSI conformance level 1 (VT420)
			// ESC SP M - ANSI conformance level 2 (VT420)
			// ESC SP N - ANSI conformance level 3 (VT420)

		case '#':
			vtInputEscapeHash(ch);
			return;

		// case '(': // select G0 94-character set
		// case ')': // select G1 94-character set
		// case '*': // select G2 94-character set (VT220)
		// case '+': // select G3 94-character set (VT220)
		// case '-': // select G1 96-character set (VT320)
		// case '.': // select G2 96-character set (VT320)
		// case '/': // select G3 96-character set (VT320)

	}
	vtState = VT100_STATE_BASE;
}

static void vtInputCSIContinue(uint8_t ch) {
	if (ch >= '0' && ch <= '9') {
		if (vtParamPtr < VT100_MAX_PARAMS) {
			vtParam[vtParamPtr] *= 10;
			vtParam[vtParamPtr] += (ch - '0');
		}
	} else if (ch == ';' || ch == ':') {
		if (vtParamPtr < VT100_MAX_PARAMS) {
			++vtParamPtr;
		}
	} else if (ch == '?') {
		vtState = VT100_STATE_CSI_PRIVATE;
	} else {
		vtEscVariant = ch;
	}
}

static void vtInputCSIBasic(uint8_t ch) {
	uint8_t i;
	switch (ch) {

		// ESC [ <n> @ - insert characters (VT220)
		case '@':
			if (!(i = vtParam[0])) ++i;
			if (cursorX < (SEGTERM_COLS<<2))
				scrollCols(cursorY, cursorX, (SEGTERM_COLS<<2), +i);
			break;

		// ESC [ <n> A - cursor up
		case 'A':
			if (!(i = vtParam[0])) ++i; cursorY -= i;
			if (cursorY > (SEGTERM_ROWS-1)) cursorY = 0;
			break;

		// ESC [ <n> B - cursor down
		case 'B':
			if (!(i = vtParam[0])) ++i; cursorY += i;
			if (cursorY > (SEGTERM_ROWS-1)) cursorY = (SEGTERM_ROWS-1);
			break;

		// ESC [ <n> C - cursor right
		case 'C':
			if (!(i = vtParam[0])) ++i; cursorX += i;
			if (cursorX > (SEGTERM_COLS<<2)) cursorX = (SEGTERM_COLS<<2);
			break;

		// ESC [ <n> D - cursor left
		case 'D':
			if (!(i = vtParam[0])) ++i; cursorX -= i;
			if (cursorX > (SEGTERM_COLS<<2)) cursorX = 0;
			break;

		// ESC [ <n> E - next line (VT510)
		case 'E':
			if (!(i = vtParam[0])) ++i; cursorX = 0;
			while (i) { vtIndex(); --i; }
			break;

		// ESC [ <n> F - previous line (VT510)
		case 'F':
			if (!(i = vtParam[0])) ++i; cursorX = 0;
			while (i) { vtReverseIndex(); --i; }
			break;

		// ESC [ <c> G - cursor X position (VT510)
		// ESC [ <n> ` - horizontal position absolute (VT510)
		// ESC [ <n> a - horizontal position relative (VT510)
		case 'G': case '`': case 'a':
			cursorX = vtParam[0]; if (cursorX) --cursorX;
			if (cursorX > (SEGTERM_COLS<<2)) cursorX = (SEGTERM_COLS<<2);
			break;

		// ESC [ <l> ; <c> H - cursor position
		// ESC [ <l> ; <c> f - horizontal and vertical position
		case 'H': case 'f':
			cursorY = vtParam[0]; if (cursorY) --cursorY;
			if (cursorY > (SEGTERM_ROWS-1)) cursorY = (SEGTERM_ROWS-1);
			cursorX = vtParam[1]; if (cursorX) --cursorX;
			if (cursorX > (SEGTERM_COLS<<2)) cursorX = (SEGTERM_COLS<<2);
			break;

		// ESC [ <n> I - tab forward (VT510)
		case 'I':
			if (!(i = vtParam[0])) ++i;
			while (i) { vtTab(); --i; }
			break;

		// ESC [ <n> J - erase in display
		//   n = 0 - clear from cursor to end of screen
		//   n = 1 - clear from beginning of screen to cursor
		//   n = 2 - clear entire screen
		case 'J':
			switch (vtParam[0]) {
				case 0: clearRows(cursorY + 1, SEGTERM_ROWS); break;
				case 1: clearRows(0, cursorY); break;
				case 2: clearRows(0, SEGTERM_ROWS); break;
			}
			// fallthrough

		// ESC [ <n> K - erase in line
		//   n = 0 - clear from cursor to end of line
		//   n = 1 - clear from beginning of line to cursor
		//   n = 2 - clear entire line
		case 'K':
			switch (vtParam[0]) {
				case 0: clearCols(cursorY, cursorX, (SEGTERM_COLS<<2)); break;
				case 1: if ((i = cursorX) < (SEGTERM_COLS<<2)) ++i; clearCols(cursorY, 0, i); break;
				case 2: clearCols(cursorY, 0, (SEGTERM_COLS<<2)); break;
			}
			break;

		// ESC [ <n> L - insert lines
		case 'L':
			if (!(i = vtParam[0])) ++i;
			if (cursorY >= scrollTop && cursorY < scrollBottom)
				scrollRows(cursorY, scrollBottom, +i);
			break;

		// ESC [ <n> M - delete lines
		case 'M':
			if (!(i = vtParam[0])) ++i;
			if (cursorY >= scrollTop && cursorY < scrollBottom)
				scrollRows(cursorY, scrollBottom, -i);
			break;

		// ESC [ <b> ; ... ; <b> N - output raw 7-segment bitmaps (SegTerm extension)
		case 'N':
			if (vtParamPtr < VT100_MAX_PARAMS) ++vtParamPtr;
			for (i = 0; i < vtParamPtr; ++i) vtPrintChar(1, vtParam[i]);
			break;

		// ESC [ <b> ; ... ; <b> O - output raw characters (SegTerm extension)
		case 'O':
			if (vtParamPtr < VT100_MAX_PARAMS) ++vtParamPtr;
			for (i = 0; i < vtParamPtr; ++i) vtPrintChar(0, vtParam[i]);
			break;

		// ESC [ <n> P - delete characters
		case 'P':
			if (!(i = vtParam[0])) ++i;
			if (cursorX < (SEGTERM_COLS<<2))
				scrollCols(cursorY, cursorX, (SEGTERM_COLS<<2), -i);
			break;

		// ESC [ <c> S - scroll up (VT420)
		case 'S':
			if (!(i = vtParam[0])) ++i;
			scrollRows(scrollTop, scrollBottom, -i);
			break;

		// ESC [ <c> T - scroll down (VT420)
		case 'T':
			if (!(i = vtParam[0])) ++i;
			scrollRows(scrollTop, scrollBottom, +i);
			break;

		// ESC [ <n> U - move <n> pages forward (VT420; not implemented)
		// ESC [ <n> V - move <n> pages backward (VT420; not implemented)

		// ESC [ <n> X - erase characters (VT220)
		case 'X':
			if (!(i = vtParam[0])) ++i;
			clearCols(cursorY, cursorX, cursorX + i);
			break;

		// ESC [ <n> Z - tab backward (VT510)
		case 'Z':
			if (!(i = vtParam[0])) ++i;
			while (i) { vtReverseTab(); --i; }
			break;

		// ESC [ <n> b - set brightness (SegTerm extension)
		case 'b':
			setBrightness(vtParam[0]);
			break;

		// ESC [ c - device attribute report
		case 'c':
			vtOutput(0x1B);
			vtOutput('[');
			vtOutput('?');
			vtOutput('6');
			vtOutput('c');
			break;

		// ESC [ <l> d - vertical position absolute (VT510)
		case 'd':
			cursorY = vtParam[0]; if (cursorY) --cursorY;
			if (cursorY > (SEGTERM_ROWS-1)) cursorY = (SEGTERM_ROWS-1);
			break;

		// ESC [ <l> e - vertical position relative (VT510)
		case 'e':
			cursorY = vtParam[0]; if (cursorY) --cursorY; cursorY += scrollTop;
			if (cursorY > (scrollBottom - 1)) cursorY = (scrollBottom - 1);
			break;

		// ESC [ <n> g - tabulation clear
		//   n = 0 - current column
		//   n = 3 - all tab stops
		case 'g':
			switch (vtParam[0]) {
				case 0: tabStops[cursorX >> 2] &=~ (1 << (cursorX & 3)); break;
				case 3: for (i = 0; i < SEGTERM_COLS; ++i) tabStops[i] = 0; break;
			}
			break;

		// ESC [ <n> ; ... ; <n> h - set mode
		//   n =  2 - keyboard lock
		//   n =  3 - show control characters (VT510; not implemented)
		//   n =  4 - insert mode
		//   n = 12 - local echo off
		//   n = 20 - newline mode
		case 'h':
			if (vtParamPtr < VT100_MAX_PARAMS) ++vtParamPtr;
			for (i = 0; i < vtParamPtr; ++i) {
				switch (vtParam[i]) {
					case  2: vtMode |=  VT100_MODE_KEYBOARD_LOCK; break;
					case  4: vtMode |=  VT100_MODE_INSERT;        break;
					case 12: vtMode &=~ VT100_MODE_LOCAL_ECHO;    break;
					case 20: vtMode |=  VT100_MODE_NEWLINE;       break;
				}
			}
			break;

		// ESC [ <n> i - media copy (not implemented)
		//   n = 0 - print screen
		//   n = 2 - send screen data through host port (VT510)
		//   n = 4 - exit printer controller
		//   n = 5 - enter printer controller
		//   n = 6 - start printer to host session (VT510)
		//   n = 7 - stop printer to host session (VT510)

		// ESC [ <n> ; <n> ; <n> j - fill screen (SegTerm extension)
		case 'j':
			if (!((i = vtParam[2]) & 0xF0)) i |= (VT100_BRIGHTNESS_NORMAL << 4);
			fillScreen(vtParam[0], vtParam[1], i);
			break;

		// ESC [ <n> ; ... ; <n> l - reset mode
		//   n =  2 - keyboard unlock
		//   n =  3 - interpret control characters (VT510; not implemented)
		//   n =  4 - replace mode
		//   n = 12 - local echo on
		//   n = 20 - linefeed mode
		case 'l':
			if (vtParamPtr < VT100_MAX_PARAMS) ++vtParamPtr;
			for (i = 0; i < vtParamPtr; ++i) {
				switch (vtParam[i]) {
					case  2: vtMode &=~ VT100_MODE_KEYBOARD_LOCK; break;
					case  4: vtMode &=~ VT100_MODE_INSERT;        break;
					case 12: vtMode |=  VT100_MODE_LOCAL_ECHO;    break;
					case 20: vtMode &=~ VT100_MODE_NEWLINE;       break;
				}
			}
			break;

		// ESC [ <n> ; ... ; <n> m - select graphic rendition
		//   (As this is the most well-known and widely-documented
		//   of all escape sequences, I don't feel the need to list
		//   all the possible values of <n> here; just look at the
		//   case statements to see what's implemented.)
		case 'm':
			if (vtParamPtr < VT100_MAX_PARAMS) ++vtParamPtr;
			for (i = 0; i < vtParamPtr; ++i) {
				switch (vtParam[i]) {
					case 0: txtAttr = 0; txtColor = 7; break;
					case 1: txtAttr |= 0x01; break;
					case 2: txtAttr |= 0x02; break;
					case 3: txtAttr |= 0x04; break;
					case 4: txtAttr |= 0x08; break;
					case 5: txtAttr |= 0x10; break;
					case 6: txtAttr |= 0x20; break;
					case 7: txtAttr |= 0x40; break;
					case 8: txtAttr |= 0x80; break;
					case 22: txtAttr &=~ 0x03; break;
					case 23: txtAttr &=~ 0x04; break;
					case 24: txtAttr &=~ 0x08; break;
					case 25: txtAttr &=~ 0x10; break;
					case 26: txtAttr &=~ 0x20; break;
					case 27: txtAttr &=~ 0x40; break;
					case 28: txtAttr &=~ 0x80; break;
					case 30: txtColor = 0; break;
					case 31: txtColor = 1; break;
					case 32: txtColor = 2; break;
					case 33: txtColor = 3; break;
					case 34: txtColor = 4; break;
					case 35: txtColor = 5; break;
					case 36: txtColor = 6; break;
					case 37: txtColor = 7; break;
					case 38: i = vtParamPtr; break;
					case 39: txtColor = 7; break;
					case 48: i = vtParamPtr; break;
					case 90: txtColor = 8; break;
					case 91: txtColor = 9; break;
					case 92: txtColor = 10; break;
					case 93: txtColor = 11; break;
					case 94: txtColor = 12; break;
					case 95: txtColor = 13; break;
					case 96: txtColor = 14; break;
					case 97: txtColor = 15; break;
				}
			}
			break;

		// ESC [ <n> n - report
		//   n = 5 - device status report
		//   n = 6 - cursor position report
		case 'n':
			switch (vtParam[0]) {
				case 5:
					vtOutput(0x1B);
					vtOutput('[');
					vtOutput('0');
					vtOutput('n');
					break;
				case 6:
					vtOutput(0x1B);
					vtOutput('[');
					vtOutputNum(cursorY + 1);
					vtOutput(';');
					vtOutputNum(cursorX + 1);
					vtOutput('R');
					break;
			}
			break;

		// ESC [ <n> p - select setup language (VT510; not implemented)
		//   n = 0 - English
		//   n = 1 - English
		//   n = 2 - French
		//   n = 3 - German
		//   n = 4 - Spanish
		//   n = 5 - Italian

		// ESC [ <n> q - keyboard LEDs
		//   n = 0 - all LEDs off
		//   n = 1 - LED 1 on
		//   n = 2 - LED 2 on
		//   n = 3 - LED 3 on
		//   n = 4 - LED 4 on
		case 'q':
			switch (vtParam[0]) {
				case 0:
#ifdef SEGTERM_KBDLED_1
					digitalWrite(SEGTERM_KBDLED_1, LOW);
#endif
#ifdef SEGTERM_KBDLED_2
					digitalWrite(SEGTERM_KBDLED_2, LOW);
#endif
#ifdef SEGTERM_KBDLED_3
					digitalWrite(SEGTERM_KBDLED_3, LOW);
#endif
#ifdef SEGTERM_KBDLED_4
					digitalWrite(SEGTERM_KBDLED_4, LOW);
#endif
					break;
#ifdef SEGTERM_KBDLED_1
				case 1:
					digitalWrite(SEGTERM_KBDLED_1, HIGH);
					break;
#endif
#ifdef SEGTERM_KBDLED_2
				case 2:
					digitalWrite(SEGTERM_KBDLED_2, HIGH);
					break;
#endif
#ifdef SEGTERM_KBDLED_3
				case 3:
					digitalWrite(SEGTERM_KBDLED_3, HIGH);
					break;
#endif
#ifdef SEGTERM_KBDLED_4
				case 4:
					digitalWrite(SEGTERM_KBDLED_4, HIGH);
					break;
#endif
			}
			break;

		// ESC [ <t> ; <b> r - set scrolling region
		case 'r':
			scrollTop = vtParam[0];
			if (scrollTop) --scrollTop;
			if (scrollTop > (SEGTERM_ROWS-2)) scrollTop = (SEGTERM_ROWS-2);
			scrollBottom = vtParam[1];
			if (!scrollBottom) scrollBottom = SEGTERM_ROWS;
			if (scrollBottom < (scrollTop+2)) scrollBottom = (scrollTop+2);
			if (scrollBottom > SEGTERM_ROWS) scrollBottom = SEGTERM_ROWS;
			break;

		// ESC [ s - save cursor position (SCO extension, VT510)
		// ESC [ <l> ; <r> s - set left and right margin (VT420; not implemented)
		case 's':
			if (!vtParam[0] && !vtParam[1]) {
				saveCursorY = cursorY;
				saveCursorX = cursorX;
			}
			break;

		// ESC [ <n> t - set lines per page (VT420; not implemented)

		// ESC [ u - restore cursor position (SCO extension, VT510)
		// ESC [ <n> ; <b> u - map font character (SegTerm extension)
		case 'u':
			if (!vtParam[0] && !vtParam[1]) {
				cursorY = saveCursorY;
				cursorX = saveCursorX;
			} else {
				setFontChar(vtParam[0], vtParam[1]);
			}
			break;

		// ESC [ <n> v - report font character (SegTerm extension)
		case 'v':
			vtOutput(0x1B);
			vtOutput('[');
			vtOutputNum(vtParam[0]);
			vtOutput(';');
			vtOutputNum(getFontChar(vtParam[0]));
			vtOutput('v');
			break;

		// ESC [ <n> w - font control (SegTerm extension)
		case 'w':
			switch (vtParam[0]) {
				case 1: loadFont(0);
				case 2: loadFont(1);
				case 10: loadFont(0);
				case 20: loadFontFromEEPROM( ((int)(vtParam[1]+1)) << 4 ); break;
				case 30: saveFontToEEPROM  ( ((int)(vtParam[1]+1)) << 4 ); break;
			}
			break;

		// ESC [ <n> x - set cursor XOR mask (SegTerm extension)
		case 'x':
			setXorMask(vtParam[0] ? vtParam[0] : SEGTERM_XORMASK_DEFAULT);
			break;

		// ESC [ <n> ; <m> y - self-test (not implemented)
		//   n = 2, m =  1 - power-on test (VT102)
		//   n = 2, m =  2 - data loopback test (VT102)
		//   n = 2, m =  4 - EIA modem control test (VT102)
		//   n = 2, m =  9 - continuous power-on test (VT102)
		//   n = 2, m = 10 - continuous data loopback test (VT102)
		//   n = 2, m = 12 - continuous EIA modem control test (VT102)
		//   n = 2, m = 16 - printer port data loopback test (VT102)
		//   n = 2, m = 24 - continuous printer port data loopback test (VT102)
		//   n = 4, m =  0 - tests m = 1, 2, 3, 6 (VT220)
		//   n = 4, m =  1 - power-on test (VT220)
		//   n = 4, m =  2 - EIA port loopback test (VT220)
		//   n = 4, m =  3 - printer port loopback test (VT220)
		//   n = 4, m =  6 - EIA port modem control line loopback test (VT220)
		//   n = 4, m =  7 - 20mA port loopback test (VT220)
		//   n = 4, m =  9 - repeat tests (VT220)
		case 'y':
			if (vtParam[0] == 7) {
				switch (vtParam[1] &~ 0x08) {
					case 1:
						do {
							uint8_t row, col;
							fillScreen(0x01, SEGTERM_CHATTR_RAW,  (VT100_BRIGHTNESS_NORMAL<<4)   ); delay(500);
							fillScreen(0x02, SEGTERM_CHATTR_RAW,  (VT100_BRIGHTNESS_NORMAL<<4)   ); delay(500);
							fillScreen(0x04, SEGTERM_CHATTR_RAW,  (VT100_BRIGHTNESS_NORMAL<<4)   ); delay(500);
							fillScreen(0x08, SEGTERM_CHATTR_RAW,  (VT100_BRIGHTNESS_NORMAL<<4)   ); delay(500);
							fillScreen(0x10, SEGTERM_CHATTR_RAW,  (VT100_BRIGHTNESS_NORMAL<<4)   ); delay(500);
							fillScreen(0x20, SEGTERM_CHATTR_RAW,  (VT100_BRIGHTNESS_NORMAL<<4)   ); delay(500);
							fillScreen(0x40, SEGTERM_CHATTR_RAW,  (VT100_BRIGHTNESS_NORMAL<<4)   ); delay(500);
							fillScreen(0x80, SEGTERM_CHATTR_RAW,  (VT100_BRIGHTNESS_NORMAL<<4)   ); delay(500);
							fillScreen(0,    0,                  ((VT100_BRIGHTNESS_NORMAL<<4)|3)); delay(500);
							fillScreen(0xFF, SEGTERM_CHATTR_RAW, ((VT100_BRIGHTNESS_NORMAL<<4)|3)); delay(500);
							fillScreen(0,    0,                   (VT100_BRIGHTNESS_NORMAL<<4)   );
							lockScreen();
							for (row = 0; row < SEGTERM_ROWS; ++row) {
								for (col = 0; col < (SEGTERM_COLS<<2); ++col) {
									setCharAndAttr(row, col, 0xFF, SEGTERM_CHATTR_RAW);
								}
								unlockScreen();
								delay(500);
								lockScreen();
								for (col = 0; col < (SEGTERM_COLS<<2); ++col) {
									setCharAndAttr(row, col, 0, 0);
								}
							}
							for (col = 0; col < (SEGTERM_COLS<<2); ++col) {
								for (row = 0; row < SEGTERM_ROWS; ++row) {
									setCharAndAttr(row, col, 0xFF, SEGTERM_CHATTR_RAW);
								}
								unlockScreen();
								delay(500);
								lockScreen();
								for (row = 0; row < SEGTERM_ROWS; ++row) {
									setCharAndAttr(row, col, 0, 0);
								}
							}
							unlockScreen();
						} while (vtParam[1] & 0x08);
						break;
				}
			}
			break;

		// ESC [ <n> z - screen control (SegTerm extension)
		case 'z':
			switch (vtParam[0]) {
				case 1: lockScreen(); break;
				case 2: unlockScreen(); break;
				case 3: while (unlockScreen()); break;
				case 10: loadDefaultDisplaySettings(); break;
				case 20: loadDisplaySettingsFromEEPROM( ((int)vtParam[1]) << 4 ); break;
				case 30: saveDisplaySettingsToEEPROM  ( ((int)vtParam[1]) << 4 ); break;
			}
			break;

		// ESC [ <dragons> ~ - function key (VT510; not implemented)

	}
	vtState = VT100_STATE_BASE;
}

static void vtInputCSIFinish(uint8_t ch) {
	switch (vtEscVariant) {

		case 0:
			vtInputCSIBasic(ch);
			return;

		// case ' ':
			// ESC [ <dragons> SP P - move to page <n> (VT420)
			// ESC [ <dragons> SP Q - move <n> pages forward (VT420)
			// ESC [ <dragons> SP R - move <n> pages backward (VT420)
			// ESC [ <dragons> SP p - set scroll speed (VT510)
			// ESC [ <dragons> SP q - set cursor style (VT510)
			// ESC [ <dragons> SP r - set key click volume (VT510)
			// ESC [ <dragons> SP t - set warning bell volume (VT510)
			// ESC [ <dragons> SP u - set margin bell volume (VT510)
			// ESC [ <dragons> SP v - set lock key style (VT510)
			// ESC [ <dragons> SP } - set keyboard language (VT510)
			// ESC [ <dragons> SP ~ - set terminal mode emulation (VT510)

		case '!':
			// ESC [ ! p - soft reset (VT220)
			if (ch == 'p') initVT100();
			break;

		// case '"':
			// ESC [ <dragons> " p - set compatibility level (VT220)
			// ESC [ <dragons> " q - select character attributes (VT220)
			// ESC [ <dragons> " t - set refresh rate (VT510)
			// ESC [ <dragons> " u - set transmit rate limit (VT510)
			// ESC [ <dragons> " v - window report (VT420)

		// case '$':
			// ESC [ <dragons> $ p - ANSI mode report (VT320)
			// ESC [ <dragons> $ q - disconnect delay time (VT510)
			// ESC [ <dragons> $ r - change attributes in rectangular area (VT420)
			// ESC [ <dragons> $ s - set printer type (VT510)
			// ESC [ <dragons> $ t - reverse attributes in rectangular area (VT420)
			// ESC [ <dragons> $ u - terminal state report (VT320)
			// ESC [ <dragons> $ v - copy rectangular area (VT420)
			// ESC [ <dragons> $ w - presentation state report (VT320)
			// ESC [ <dragons> $ x - fill rectangular area (VT420)
			// ESC [ <dragons> $ y - ANSI report mode (VT510)
			// ESC [ <dragons> $ z - erase rectangular area (VT420)
			// ESC [ <dragons> $ { - selective erase rectangular area (VT420)
			// ESC [ <dragons> $ | - set columns per page (VT420)
			// ESC [ <dragons> $ } - set active status display (VT320)
			// ESC [ <dragons> $ ~ - set status line type (VT320)

		// case '&':
			// ESC [ & u - user-preferred supplemental set report (VT320)
			// ESC [ & x - enable session management (VT420)

		// case '\'':
			// ESC [ <dragons> ' } - insert columns (VT420)
			// ESC [ <dragons> ' ~ - delete columns (VT420)

		// case ')':
			// ESC [ <dragons> ) p - set printed data type (VT510)

		// case '*':
			// ESC [ <dragons> * p - set ProPrinter character set (VT510)
			// ESC [ <dragons> * q - secure reset confirmation (VT420)
			// ESC [ <dragons> * r - communication speed (VT510)
			// ESC [ <dragons> * s - flow control (VT510)
			// ESC [ <dragons> * u - communication port (VT510)
			// ESC [ <dragons> * x - select attribute change extent (VT420)
			// ESC [ <dragons> * y - checksum of rectangular area (VT420)
			// ESC [ <dragons> * z - invoke macro (VT420)
			// ESC [ <dragons> * { - macro space report (VT510)
			// ESC [ <dragons> * | - set lines per screen (VT510)
			// ESC [ <dragons> * } - local function key control (VT420)

		// case '+':
			// ESC [ <dragons> + p - secure reset (VT420)
			// ESC [ <dragons> + q - enable local functions (VT420)
			// ESC [ <dragons> + r - select modifier key reporting (VT420)
			// ESC [ <dragons> + w - set port parameters (VT510)
			// ESC [ <dragons> + x - program key free memory inquiry (VT510)
			// ESC [ <dragons> + y - program key free memory report (VT510)
			// ESC [ <dragons> + z - program key action (VT510)

		// case ',':
			// ESC [ <dragons> , p - set clock (VT510)
			// ESC [ <dragons> , q - set terminal ID (VT510)
			// ESC [ <dragons> , u - request key type (VT510)
			// ESC [ <dragons> , v - report key type (VT510)

		// case '=':
			// ESC [ = c - tertiary device attribute report (VT420)

		// case '>':
			// ESC [ > c - secondary device attribute report (VT320)

	}
	vtState = VT100_STATE_BASE;
}

static void vtInputCSIBasicPrivate(uint8_t ch) {
	uint8_t i;
	switch (ch) {

		// ESC [ ? <n> J - selective erase in display (VT220; not implemented)
		//   n = 0 - clear from cursor to end of screen
		//   n = 1 - clear from beginning of screen to cursor
		//   n = 2 - clear entire screen

		// ESC [ ? <n> K - selective erase in line (VT220; not implemented)
		//   n = 0 - clear from cursor to end of line
		//   n = 1 - clear from beginning of line to cursor
		//   n = 2 - clear entire line

		// ESC [ ? 2 W - set tab every 2 columns (SegTerm extension)
		// ESC [ ? 4 W - set tab every 4 columns (SegTerm extension)
		// ESC [ ? 5 W - set tab every 8 columns (VT510)
		case 'W':
			switch (vtParam[0]) {
				case 2: for(i=0;i<SEGTERM_COLS;++i) tabStops[i] = 5;      break;
				case 4: for(i=0;i<SEGTERM_COLS;++i) tabStops[i] = 1;      break;
				case 5: for(i=0;i<SEGTERM_COLS;++i) tabStops[i] = !(i&1); break;
			}
			break;

		// ESC [ ? <n> ; ... ; <n> h - set mode (private)
		//   n =   1 - application cursor keys (not implemented)
		//   n =   2 - ANSI mode
		//   n =   3 - 132 columns (not implemented)
		//   n =   4 - smooth scrolling (not implemented)
		//   n =   5 - reverse screen (not implemented)
		//   n =   6 - relative origin (not implemented)
		//   n =   7 - auto wrap on (not implemented)
		//   n =   8 - auto repeat on (not implemented)
		//   n =  12 - cursor blink on (appears to be a Microsoft extension)
		//   n =  18 - print form feed on (not implemented)
		//   n =  19 - print extent full screen (not implemented)
		//   n =  25 - text cursor on (VT220)
		//   n =  34 - right-to-left mode on (VT510; not implemented)
		//   n =  35 - Hebrew keyboard mode (VT510; not implemented)
		//   n =  36 - 8-bit Hebrew encoding (VT510; not implemented)
		//   n =  42 - national mode (VT220; not implemented)
		//   n =  57 - North American keyboard mode (VT510; not implemented)
		//   n =  58 - enter ProPrinter emulation mode (VT510; not implemented)
		//   n =  61 - vertical cursor coupling mode on (VT420; not implemented)
		//   n =  64 - page cursor coupling mode on (VT420; not implemented)
		//   n =  66 - application keypad mode (VT420; not implemented)
		//   n =  67 - backarrow produces backspace (VT420; not implemented)
		//   n =  68 - data processing mode (VT320; not implemented)
		//   n =  69 - vertical split screen mode on (VT420; not implemented)
		//   n =  73 - limited transmit rate (VT510; not implemented)
		//   n =  81 - key position reports (VT420; not implemented)
		//   n =  95 - no clear on column change (VT510; not implemented)
		//   n =  96 - right-to-left copy on (VT510; not implemented)
		//   n =  97 - CRT saver on (VT510; not implemented)
		//   n =  98 - auto resize mode on (VT510; not implemented)
		//   n =  99 - modem control mode on (VT510; not implemented)
		//   n = 100 - auto answerback on (VT510; not implemented)
		//   n = 101 - conceal answerback message on (VT510; not implemented)
		//   n = 102 - ignore NUL (VT510; not implemented)
		//   n = 103 - half duplex mode (VT510; not implemented)
		//   n = 104 - secondary keyboard mode (VT510; not implemented)
		//   n = 106 - enable overscan (VT510; not implemented)
		//   n = 108 - num lock on (VT510; not implemented)
		//   n = 109 - caps lock on (VT510; not implemented)
		//   n = 110 - keyboard LED host indicator mode on (VT510; not implemented)
		//   n = 170 - lowercase on (SegTerm extension)
		//   n = 171 - decimal points on lowercase on (SegTerm extension)
		//   n = 172 - decimal points on uppercase on (SegTerm extension)
		//   n = 178 - decimal points on high bits on (SegTerm extension)
		case 'h':
			if (vtParamPtr < VT100_MAX_PARAMS) ++vtParamPtr;
			for (i = 0; i < vtParamPtr; ++i) {
				switch (vtParam[i]) {
					case   2: vtMode &=~ VT100_MODE_VT52;         break;
					case  12: vtMode |=  VT100_MODE_CURSOR_BLINK; break;
					case  25: vtMode |=  VT100_MODE_CURSOR_ON;    break;
					case 170: setLcMode(1); break;
					case 171: setDotMode(getDotMode() | 0x01); break;
					case 172: setDotMode(getDotMode() | 0x02); break;
					case 173: setDotMode(getDotMode() | 0x04); break;
					case 174: setDotMode(getDotMode() | 0x08); break;
					case 175: setDotMode(getDotMode() | 0x10); break;
					case 176: setDotMode(getDotMode() | 0x20); break;
					case 177: setDotMode(getDotMode() | 0x40); break;
					case 178: setDotMode(getDotMode() | 0x80); break;
				}
			}
			break;

		// ESC [ ? <n> i - media copy (not implemented)
		//   n =  1 - print cursor line
		//   n =  4 - exit auto print
		//   n =  5 - enter auto print
		//   n =  8 - stop printer-to-host session (VT420)
		//   n =  9 - start printer-to-host session (VT420)
		//   n = 10 - print composed main display (VT420)
		//   n = 11 - print all pages (VT420)
		//   n = 18 - assign printer to active session (VT420)
		//   n = 19 - release printer (VT420)

		// ESC [ ? <n> ; ... ; <n> l - reset mode (private)
		//   n =   1 - standard cursor keys (not implemented)
		//   n =   2 - VT52 mode
		//   n =   3 - 80 columns (not implemented)
		//   n =   4 - jump scrolling (not implemented)
		//   n =   5 - normal screen (not implemented)
		//   n =   6 - absolute origin (not implemented)
		//   n =   7 - auto wrap off (not implemented)
		//   n =   8 - auto repeat off (not implemented)
		//   n =  12 - cursor blink off (appears to be a Microsoft extension)
		//   n =  18 - print form feed off (not implemented)
		//   n =  19 - print extent scrolling region (not implemented)
		//   n =  25 - text cursor off (VT220)
		//   n =  34 - right-to-left mode off (VT510; not implemented)
		//   n =  35 - North American keyboard mode (VT510; not implemented)
		//   n =  36 - 7-bit Hebrew encoding (VT510; not implemented)
		//   n =  42 - international mode (VT220; not implemented)
		//   n =  57 - Greek keyboard mode (VT510; not implemented)
		//   n =  58 - exit ProPrinter emulation mode (VT510; not implemented)
		//   n =  61 - vertical cursor coupling mode off (VT420; not implemented)
		//   n =  64 - page cursor coupling mode off (VT420; not implemented)
		//   n =  66 - numeric keypad mode (VT420; not implemented)
		//   n =  67 - backarrow produces delete (VT420; not implemented)
		//   n =  68 - typewriter mode (VT320; not implemented)
		//   n =  69 - vertical split screen mode off (VT420; not implemented)
		//   n =  73 - unlimited transmit rate (VT510; not implemented)
		//   n =  81 - character code reports (VT420; not implemented)
		//   n =  95 - clear on column change (VT510; not implemented)
		//   n =  96 - right-to-left copy off (VT510; not implemented)
		//   n =  97 - CRT saver off (VT510; not implemented)
		//   n =  98 - auto resize mode off (VT510; not implemented)
		//   n =  99 - modem control mode off (VT510; not implemented)
		//   n = 100 - auto answerback off (VT510; not implemented)
		//   n = 101 - conceal answerback message off (VT510; not implemented)
		//   n = 102 - accept NUL (VT510; not implemented)
		//   n = 103 - full duplex mode (VT510; not implemented)
		//   n = 104 - primary keyboard mode (VT510; not implemented)
		//   n = 106 - disable overscan (VT510; not implemented)
		//   n = 108 - num lock off (VT510; not implemented)
		//   n = 109 - caps lock off (VT510; not implemented)
		//   n = 110 - keyboard LED host indicator mode off (VT510; not implemented)
		//   n = 170 - lowercase off (SegTerm extension)
		//   n = 171 - decimal points on lowercase off (SegTerm extension)
		//   n = 172 - decimal points on uppercase off (SegTerm extension)
		//   n = 178 - decimal points on high bits off (SegTerm extension)
		case 'l':
			if (vtParamPtr < VT100_MAX_PARAMS) ++vtParamPtr;
			for (i = 0; i < vtParamPtr; ++i) {
				switch (vtParam[i]) {
					case   2: vtMode |=  VT100_MODE_VT52;         break;
					case  12: vtMode &=~ VT100_MODE_CURSOR_BLINK; break;
					case  25: vtMode &=~ VT100_MODE_CURSOR_ON;    break;
					case 170: setLcMode(0); break;
					case 171: setDotMode(getDotMode() &~ 0x01); break;
					case 172: setDotMode(getDotMode() &~ 0x02); break;
					case 173: setDotMode(getDotMode() &~ 0x04); break;
					case 174: setDotMode(getDotMode() &~ 0x08); break;
					case 175: setDotMode(getDotMode() &~ 0x10); break;
					case 176: setDotMode(getDotMode() &~ 0x20); break;
					case 177: setDotMode(getDotMode() &~ 0x40); break;
					case 178: setDotMode(getDotMode() &~ 0x80); break;
				}
			}
			break;

		// ESC [ ? <n> n - report (private)
		//   n =   6 - extended cursor position report (VT420; not implemented)
		//   n =  15 - report printer status (not implemented)
		//   n =  25 - report user-defined keys (VT220; not implemented)
		//   n =  26 - report keyboard language (VT220; not implemented)
		//   n =  62 - macro space (VT420; not implemented)
		//   n =  63 - memory checksum (VT420; not implemented)
		//   n =  75 - data integrity (VT420; not implemented)
		//   n =  85 - multiple session status (VT420; not implemented)
		//   n = 177 - report scrolling region (SegTerm extension)
		//   n = 178 - report window size (SegTerm extension)
		//   n = 179 - report free RAM (SegTerm extension)
		case 'n':
			switch (vtParam[0]) {
				case 177:
					vtOutput(0x1B);
					vtOutput('[');
					vtOutputNum(scrollTop + 1);
					vtOutput(';');
					vtOutputNum(scrollBottom);
					vtOutput('R');
					break;
				case 178:
					vtOutput(0x1B);
					vtOutput('[');
					vtOutputNum(SEGTERM_ROWS);
					vtOutput(';');
					vtOutputNum(SEGTERM_COLS << 2);
					vtOutput('R');
					break;
				case 179:
					vtOutput(0x1B);
					vtOutput('[');
					vtOutputNum16((uint16_t)freeRAM());
					vtOutput('R');
					break;
			}
			break;

		// ESC [ ? <dragons> r - enter/exit PCTerm mode (VT510; not implemented)

	}
	vtState = VT100_STATE_BASE;
}

static void vtInputCSIFinishPrivate(uint8_t ch) {
	switch (vtEscVariant) {

		case 0:
			vtInputCSIBasicPrivate(ch);
			return;

		// case '$':
			// ESC [ ? <dragons> $ p - private mode report (VT320)
			// ESC [ ? <dragons> $ y - private mode report (VT510)

	}
	vtState = VT100_STATE_BASE;
}

static void vtInputPrintable(uint8_t ch) {
	switch (vtState) {
		case VT100_STATE_BASE:
			vtPrintChar(0, ch);
			break;
		case VT100_STATE_ESC:
			if (vtMode & VT100_MODE_VT52) {
				vtInputEscapeVT52(ch);
			} else if (ch < 0x30) {
				vtEscVariant = ch;
			} else {
				vtInputEscapeFinish(ch);
			}
			break;
		case VT100_STATE_VT52_DCA:
			vtInputCursorAddressVT52(ch);
			break;
		case VT100_STATE_DCS:
		case VT100_STATE_SOS:
			break;
		case VT100_STATE_CSI:
			if (ch < 0x40) {
				vtInputCSIContinue(ch);
			} else {
				vtInputCSIFinish(ch);
			}
			break;
		case VT100_STATE_CSI_PRIVATE:
			if (ch < 0x40) {
				vtInputCSIContinue(ch);
			} else {
				vtInputCSIFinishPrivate(ch);
			}
			break;
		case VT100_STATE_OSC:
		case VT100_STATE_PM:
		case VT100_STATE_APC:
			break;
	}
}

static void vtInputChar(uint8_t ch) {
	if (ch < 0x20) {
		vtInputControl(ch);
	} else if (ch < 0x80) {
		vtInputPrintable(ch);
	} else if (ch < 0xA0) {
		vtInputControl(0x1B);
		vtInputPrintable((ch & 0x1F) | 0x40);
	} else {
		vtInputPrintable(ch);
	}
}

/****************************** CURSOR CONTROL ******************************/

static void toggleCursor() {
	vtMode ^= VT100_MODE_CURSOR_ACTIVE;
	if (cursorY < SEGTERM_ROWS && cursorX < (SEGTERM_COLS<<2)) {
		uint8_t a = getChAttr(cursorY, cursorX);
		a ^= SEGTERM_CHATTR_XORMASK;
		setChAttr(cursorY, cursorX, a);
	}
}

/******************************** PUBLIC API ********************************/

uint8_t vtGetMode()       { return vtMode;   }
uint8_t vtGetCursorY()    { return cursorY;  }
uint8_t vtGetCursorX()    { return cursorX;  }
uint8_t vtGetAttributes() { return txtAttr;  }
uint8_t vtGetTextColor()  { return txtColor; }

void vtSetMode(uint8_t mode) {
	vtMode = mode;
}

void vtSetCursor(uint8_t y, uint8_t x) {
	if (vtMode & VT100_MODE_CURSOR_ACTIVE) toggleCursor();
	cursorY = (y <= (SEGTERM_ROWS-1)) ? y : (SEGTERM_ROWS-1);
	cursorX = (x <= (SEGTERM_COLS<<2)) ? x : (SEGTERM_COLS<<2);
}

void vtSetCursorY(uint8_t y) {
	if (vtMode & VT100_MODE_CURSOR_ACTIVE) toggleCursor();
	cursorY = (y <= (SEGTERM_ROWS-1)) ? y : (SEGTERM_ROWS-1);
}

void vtSetCursorX(uint8_t x) {
	if (vtMode & VT100_MODE_CURSOR_ACTIVE) toggleCursor();
	cursorX = (x <= (SEGTERM_COLS<<2)) ? x : (SEGTERM_COLS<<2);
}

void vtSetAttrAndColor(uint8_t attr, uint8_t color) {
	txtAttr = attr;
	txtColor = color;
}

void vtSetAttributes(uint8_t attr) {
	txtAttr = attr;
}

void vtSetTextColor(uint8_t color) {
	txtColor = color;
}

void vtPutChar(uint8_t ch) {
	if (vtMode & VT100_MODE_CURSOR_ACTIVE) toggleCursor();
	vtInputChar(ch);
}

void vtPrint(const char * s) {
	if (vtMode & VT100_MODE_CURSOR_ACTIVE) toggleCursor();
	while (*s) { vtInputChar((uint8_t)(*s)); ++s; }
}

void vtPrintln(const char * s) {
	if (vtMode & VT100_MODE_CURSOR_ACTIVE) toggleCursor();
	while (*s) { vtInputChar((uint8_t)(*s)); ++s; }
	cursorX = 0; vtIndex(); vtState = VT100_STATE_BASE;
}

uint8_t vtGetChar() {
	if (vtOutBufStart == vtOutBufEnd) {
		return 0;
	} else {
		uint8_t ch = vtOutBuf[vtOutBufStart];
		++vtOutBufStart;
		if (vtOutBufStart >= VT100_OUTBUF_SIZE) {
			vtOutBufStart = 0;
		}
		return ch;
	}
}

void vtIdle() {
	if (vtMode & VT100_MODE_CURSOR_ON) {
		if (vtMode & VT100_MODE_CURSOR_BLINK) {
			if ((millis() - lastBlinkTime) >= VT100_CURSOR_BLINK_TIME) {
				lastBlinkTime = millis();
				toggleCursor();
			}
		} else {
			if (!(vtMode & VT100_MODE_CURSOR_ACTIVE)) {
				toggleCursor();
			}
		}
	}
}
