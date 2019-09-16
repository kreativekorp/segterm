#include <Arduino.h>
#include <inttypes.h>
#include "KeyboardLib.h"
#include "SegTerm_buttons.h"
#include "SegTerm_config.h"
#include "SegTerm_display.h"
#include "SegTerm_rtc.h"
#include "STApp_clock.h"

bool clock_setup() {
	clearRows(0, SEGTERM_ROWS);
}

bool clock_loop() {
	uint8_t col = random(SEGTERM_COLS) << 2;
	uint8_t row = random(SEGTERM_ROWS);
	
	unsigned long start = millis();
	while (millis() - start < 5000) {
		uint8_t c, y, m, d, w, hr, min, sec, ampm;
		getTime(&c, &y, &m, &d, &w, &hr, &min, &sec, &ampm, true);
		setChar(row, (col|0), ((hr >> 4) ? ('0' | (hr  >> 4)) : 0));
		setChar(row, (col|1),              ('0' | (hr  & 15))     );
		setChar(row, (col|2),              ('0' | (min >> 4))     );
		setChar(row, (col|3),              ('0' | (min & 15))     );
		setChAttr(row, (col|3), ((ampm == PM) ? SEGTERM_CHATTR_DOT : 0));
		setMdColon(row, (col>>2), ((sec & 1) ? 0 : 3));
		
		if (readKeyboardEvent() == (KBD_KEY_ESC | KBD_PRESSED)) {
			while (readKeyboardEvent() != (KBD_KEY_ESC | KBD_RELEASED));
			return false;
		}
		if (getButtons() & BUTTON_2) {
			delay(KBD_DEBOUNCE_DELAY);
			while (getButtons() & BUTTON_2);
			delay(KBD_DEBOUNCE_DELAY);
			return false;
		}
	}
	
	setChar(row, (col|0), 0);
	setChar(row, (col|1), 0);
	setChar(row, (col|2), 0);
	setChar(row, (col|3), 0);
	setChAttr(row, (col|3), 0);
	setMdColon(row, (col>>2), 0);
}

void clock_quit() {
	clearRows(0, SEGTERM_ROWS);
}
