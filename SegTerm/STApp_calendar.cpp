#include <Arduino.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include "KeyboardLib.h"
#include "SegTerm_buttons.h"
#include "SegTerm_config.h"
#include "SegTerm_display.h"
#include "SegTerm_eeprom.h"
#include "SegTerm_rtc.h"
#include "STApp_calendar.h"

static const uint8_t DAY_NAMES[] PROGMEM = {
	'?','S','M','T','W','T','F','S',
	'?','u','o','u','e','h','r','a',
	'?','n','n','e','d','u','i','t',
	'?','d','d','s','n','r','d','u',
	'?','a','a','d','e','s','a','r',
	'?','y','y','a','s','d','y','d',
	'?', 0 , 0 ,'y','d','a', 0 ,'a',
	'?', 0 , 0 , 0 ,'a','y', 0 ,'y',
	'?', 0 , 0 , 0 ,'y', 0 , 0 , 0 ,
	 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
};

static const uint8_t MONTH_NAMES[] PROGMEM = {
	'?','J','F','M','A','M','J','J','A','S','O','N','D','U','D','?',
	'?','a','e','a','p','a','u','u','u','e','c','o','e','n','u','?',
	'?','n','b','r','r','y','n','l','g','p','t','v','c','d','o','?',
	'?','u','r','c','i', 0 ,'e','y','u','t','o','e','e','e','d','?',
	'?','a','u','h','l', 0 , 0 , 0 ,'s','e','b','m','m','c','e','?',
	'?','r','a', 0 , 0 , 0 , 0 , 0 ,'t','m','e','b','b','i','c','?',
	'?','y','r', 0 , 0 , 0 , 0 , 0 , 0 ,'b','r','e','e','m','i','?',
	'?', 0 ,'y', 0 , 0 , 0 , 0 , 0 , 0 ,'e', 0 ,'r','r','b','m','?',
	'?', 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'r', 0 , 0 , 0 ,'e','b','?',
	'?', 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'r','e','?',
	'?', 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'r','?',
	 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
};

static void drawTime() {
	uint8_t c, y, m, d, w, hr, min, sec, ampm;
	getTime(&c, &y, &m, &d, &w, &hr, &min, &sec, &ampm, true);
	
	if (SEGTERM_COLS >= 1) {
		setMdColon(0, 0, ((sec & 1) ? 0 : 3));
		setChar(0, 0, ((hr >> 4) ? ('0' | (hr  >> 4)) : 0));
		setChar(0, 1,              ('0' | (hr  & 15))     );
		setChar(0, 2,              ('0' | (min >> 4))     );
		setChar(0, 3,              ('0' | (min & 15))     );
	}
	if (SEGTERM_COLS == 1) {
		setChAttr(0, 3, ((ampm == PM) ? SEGTERM_CHATTR_DOT : 0));
	}
	if (SEGTERM_COLS >= 2) {
		setChAttr(0, 3, ((sec & 1) ? 0 : SEGTERM_CHATTR_DOT));
		setChar(0, 4, ('0' | (sec >> 4)));
		setChar(0, 5, ('0' | (sec & 15)));
		setChar(0, 6, (ampm ? ((ampm == PM) ? 'P' : 'A') : 0));
		setChar(0, 7, (ampm ? (           'M'          ) : 0));
	}
	if (SEGTERM_COLS >= 3) {
		setChar(0,  9, pgm_read_byte(&DAY_NAMES[w|000]));
		setChar(0, 10, pgm_read_byte(&DAY_NAMES[w|010]));
		setChar(0, 11, pgm_read_byte(&DAY_NAMES[w|020]));
	}
	if (SEGTERM_COLS >= 4) {
		setChar(0, 13, ((d >> 4) ? ('0' | (d >> 4)) : 0));
		setChar(0, 14,             ('0' | (d & 15))     );
	}
	if (SEGTERM_COLS >= 5) {
		setChar(0, 16, pgm_read_byte(&MONTH_NAMES[m|0x00]));
		setChar(0, 17, pgm_read_byte(&MONTH_NAMES[m|0x10]));
		setChar(0, 18, pgm_read_byte(&MONTH_NAMES[m|0x20]));
	}
	if (SEGTERM_COLS >= 6) {
		setChar(0, 20, ('0' | (c >> 4)));
		setChar(0, 21, ('0' | (c & 15)));
		setChar(0, 22, ('0' | (y >> 4)));
		setChar(0, 23, ('0' | (y & 15)));
	}
	if (SEGTERM_COLS >= 7) {
		setChar(0, 24, '-');
		setChar(0, 25, ('0' | (m >> 4)));
		setChar(0, 26, ('0' | (m & 15)));
	}
	if (SEGTERM_COLS >= 8) {
		setChar(0, 27, '-');
		setChar(0, 28, ('0' | (d >> 4)));
		setChar(0, 29, ('0' | (d & 15)));
		setChar(0, 31, ('0' | (w     )));
	}
}

static void drawCalendarTitle(uint8_t row, uint8_t c, uint8_t y, uint8_t m) {
	uint8_t buf[20];
	uint8_t p1, p2, ch;
	
	// Format string with month name and year.
	p1 = 0;
	for (p2 = m; (ch = pgm_read_byte(&MONTH_NAMES[p2])); p2 += 0x10) {
		buf[p1] = ch; p1++;
	}
	buf[p1] = ' '; p1++;
	buf[p1] = '0' | (c / 10); p1++;
	buf[p1] = '0' | (c % 10); p1++;
	buf[p1] = '0' | (y / 10); p1++;
	buf[p1] = '0' | (y % 10); p1++;
	buf[p1] = 0;
	
	// Print centered.
	p2 = ((SEGTERM_COLS << 2) - p1) >> 1;
	for (p1 = 0; (ch = buf[p1]); p1++) {
		setChar(row, p2, ch); p2++;
	}
}

static void drawCalendarHeader(uint8_t row, uint8_t db, uint8_t ds, uint8_t fd) {
	for (uint8_t i = 0; i < 7; i++) {
		fd++; if (fd > 7) fd -= 7;
		setChar(row, db, pgm_read_byte(&DAY_NAMES[fd|000])); db++;
		setChar(row, db, pgm_read_byte(&DAY_NAMES[fd|010])); db++;
		db += ds;
	}
}

static void drawCalendar(uint8_t c, uint8_t y, uint8_t m, uint8_t d, uint8_t fd) {
	if (SEGTERM_ROWS >= 8 && SEGTERM_COLS >= 4) {
		uint8_t row = (
			(SEGTERM_ROWS < 10) ? 1 :
			(SEGTERM_ROWS < 14) ? 2 :
			(SEGTERM_ROWS < 16) ? 3 :
			4
		);
		uint8_t db = (
			(SEGTERM_COLS == 4) ? 1 :
			(SEGTERM_COLS == 5) ? 0 :
			(SEGTERM_COLS == 6) ? 2 :
			(SEGTERM_COLS == 7) ? 1 :
			3
		);
		uint8_t ds = (
			(SEGTERM_COLS <= 4) ? 0 :
			(SEGTERM_COLS <= 6) ? 1 :
			2
		);
		uint8_t dm = daysInMonth(c, y, m, false);
		uint8_t dw = (dayOfWeek(c, y, m, 1, false) + 6 - fd) % 7;
		uint8_t dx = (ds + 2) * dw + db;
		uint8_t cd, d1, d2, da;
		
		drawCalendarTitle(row, c, y, m);
		row += ((SEGTERM_ROWS <= 10) ? 1 : 2);
		
		if (SEGTERM_ROWS > 8) {
			drawCalendarHeader(row, db, ds, fd);
			row++;
		}
		
		for (cd = 1; cd <= dm; cd++) {
			d1 = (cd < 10) ? 0 : ('0' | (cd / 10));
			d2 = ('0' | (cd % 10));
			da = (cd == d) ? SEGTERM_CHATTR_DOT : 0;
			setCharAndAttr(row, dx, d1, da); dx++;
			setCharAndAttr(row, dx, d2, da); dx++;
			dx += ds; dw++;
			if (dw >= 7) {
				dw = 0;
				dx = db;
				row++;
			}
		}
	}
}

static uint8_t firstDay;
static uint8_t cc, cy, cm;
static uint8_t drawnDay;
static unsigned long drawnAt;

static void goToday() {
	uint8_t c, y, m, d, w, hr, min, sec, ampm;
	getTime(&c, &y, &m, &d, &w, &hr, &min, &sec, &ampm, false);
	
	if (cc == c && cy == y && cm == m) {
		if (drawnDay == d) return;
	} else {
		cc = c; cy = y; cm = m;
		clearRows(1, SEGTERM_ROWS);
	}
	
	drawCalendar(cc, cy, cm, d, firstDay);
	drawnDay = d;
	drawnAt = millis();
}

static void goAnotherDay() {
	uint8_t c, y, m, d, w, hr, min, sec, ampm;
	getTime(&c, &y, &m, &d, &w, &hr, &min, &sec, &ampm, false);
	
	if (cc != c || cy != y || cm != m) d = 0;
	clearRows(1, SEGTERM_ROWS);
	
	drawCalendar(cc, cy, cm, d, firstDay);
	drawnDay = d;
	drawnAt = millis();
}

static void goPrevMonth() {
	cm--;
	if (cm < 1) {
		cm = 12;
		cy--;
		if (cy > 99) {
			cy = 99;
			cc--;
			if (cc > 99) {
				cc = 0;
				cy = 0;
				cm = 1;
			}
		}
	}
	goAnotherDay();
}

static void goNextMonth() {
	cm++;
	if (cm > 12) {
		cm = 1;
		cy++;
		if (cy > 99) {
			cy = 0;
			cc++;
			if (cc > 99) {
				cc = 99;
				cy = 99;
				cm = 12;
			}
		}
	}
	goAnotherDay();
}

static void goPrevYear() {
	cy--;
	if (cy > 99) {
		cy = 99;
		cc--;
		if (cc > 99) {
			cc = 0;
			cy = 0;
		}
	}
	goAnotherDay();
}

static void goNextYear() {
	cy++;
	if (cy > 99) {
		cy = 0;
		cc++;
		if (cc > 99) {
			cc = 99;
			cy = 99;
		}
	}
	goAnotherDay();
}

bool calendar_setup() {
	firstDay = EEPROM.read(EE_FIRSTDAY);
	if (firstDay > 7) firstDay = 0;
	cc = cy = cm = 0;
	
	clearRows(0, 1);
	drawTime();
	goToday();
	return true;
}

bool calendar_loop() {
	uint8_t ch;
	
	drawTime();
	if (drawnDay || ((millis() - drawnAt) >= CAL_TODAY_AFTER)) goToday();
	
	if ((ch = readKeyboardEvent())) {
		if (ch == (KBD_KEY_ESC | KBD_PRESSED)) {
			while (readKeyboardEvent() != (KBD_KEY_ESC | KBD_RELEASED));
			return false;
		}
		if (ch & KBD_PRESSED) {
			ch &= KBD_KEY;
			switch (ch) {
				case KBD_KEY_ENTER: goToday();     break;
				case KBD_KEY_SPACE: goToday();     break;
				case KBD_KEY_LEFT:  goPrevMonth(); break;
				case KBD_KEY_RIGHT: goNextMonth(); break;
				case KBD_KEY_UP:    goNextYear();  break;
				case KBD_KEY_DOWN:  goPrevYear();  break;
				default: return true;
			}
			ch |= KBD_RELEASED;
			while (readKeyboardEvent() != ch);
		}
	}
	if ((ch = getButtons())) {
		unsigned long start = millis();
		switch (ch) {
			case BUTTON_1: goNextMonth(); break;
			case BUTTON_2: goToday();     break;
			case BUTTON_3: goPrevMonth(); break;
		}
		delay(KBD_DEBOUNCE_DELAY);
		while (getButtons() & ch);
		delay(KBD_DEBOUNCE_DELAY);
		if ((ch & BUTTON_2) && (millis() - start) >= CAL_LONG_PRESS) {
			return false;
		}
	}
	return true;
}

void calendar_quit() {
	clearRows(0, SEGTERM_ROWS);
}
