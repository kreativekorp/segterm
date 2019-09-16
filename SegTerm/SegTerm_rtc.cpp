#include <EEPROM.h>
#include <Wire.h>
#include <inttypes.h>
#include "SegTerm_eeprom.h"
#include "SegTerm_rtc.h"

uint8_t daysInMonth(
	uint8_t c, uint8_t y, uint8_t m,
	uint8_t bcd
) {
	if (bcd) {
		c = ((c >> 4) * 10) + (c & 0xF);
		y = ((y >> 4) * 10) + (y & 0xF);
		m = ((m >> 4) * 10) + (m & 0xF);
	}

	switch (m) {
		case 2: return ((y & 3) || (!y && (c & 3))) ? 28 : 29;
		case 4: case 6: case 9: case 11: return 30;
		default: return 31;
	}
}

uint8_t dayOfWeek(
	uint8_t c, uint8_t y, uint8_t m, uint8_t d,
	uint8_t bcd
) {
	if (bcd) {
		c = ((c >> 4) * 10) + (c & 0xF);
		y = ((y >> 4) * 10) + (y & 0xF);
		m = ((m >> 4) * 10) + (m & 0xF);
		d = ((d >> 4) * 10) + (d & 0xF);
	}

	uint8_t w = 6 - ((c & 3) << 1) + (y >> 2) + y + d;
	switch (m) {
		case 9: case 12: w++; case 6: w++;
		case 2: case 3: case 11: w++;
		case 8: w++; case 5: w++;
		case 1: case 10: w++;
	}
	if ((m < 3) && !(y & 3) && (y || !(c & 3))) w--;
	return (w %= 7) ? w : 7;
}

void setTime(
	uint8_t c, uint8_t y, uint8_t m, uint8_t d,
	uint8_t hr, uint8_t min, uint8_t sec, uint8_t ampm,
	uint8_t bcd
) {
	uint8_t w = dayOfWeek(c, y, m, d, bcd);
	if (!bcd) {
		c   = ((c   / 10) << 4) | (c   % 10);
		y   = ((y   / 10) << 4) | (y   % 10);
		m   = ((m   / 10) << 4) | (m   % 10);
		d   = ((d   / 10) << 4) | (d   % 10);
		hr  = ((hr  / 10) << 4) | (hr  % 10);
		min = ((min / 10) << 4) | (min % 10);
		sec = ((sec / 10) << 4) | (sec % 10);
	}
	if (ampm == AM) hr |= 0x40;
	if (ampm == PM) hr |= 0x60;

	Wire.beginTransmission(RTC_ADDR);
	Wire.write(0);
	Wire.write(sec);
	Wire.write(min);
	Wire.write(hr);
	Wire.write(w);
	Wire.write(d);
	Wire.write(m);
	Wire.write(y);
	Wire.endTransmission();

	EEPROMwrite(EE_CENTURY, c);

	Wire.beginTransmission(RTC_ADDR);
	Wire.write(0x0E);
	Wire.write(0x1C);
	Wire.endTransmission();
}

void getTime(
	uint8_t * c, uint8_t * y, uint8_t * m, uint8_t * d, uint8_t * w,
	uint8_t * hr, uint8_t * min, uint8_t * sec, uint8_t * ampm,
	uint8_t bcd
) {
	Wire.beginTransmission(RTC_ADDR);
	Wire.write(0);
	Wire.endTransmission();
	Wire.requestFrom(RTC_ADDR, 7);
	*sec = Wire.read();
	*min = Wire.read();
	*hr  = Wire.read();
	*w   = Wire.read();
	*d   = Wire.read();
	*m   = Wire.read();
	*y   = Wire.read();
	*c   = EEPROM.read(EE_CENTURY);

	if (((*c & 0xF0) >= 0xA0) || ((*c & 0x0F) >= 0x0A)) { *c = 0x20; }
	if (*m & 0x80) { ++*c; if ((*c & 15) >= 10) *c += 6; *m &= 0x7F; }
	if (*hr & 0x40) { *ampm = ((*hr & 0x20) ? PM : AM); *hr &= 0x1F; }
	else { *ampm = MIL; }

	if (!bcd) {
		*c   = ((*c   >> 4) * 10) + (*c   & 0xF);
		*y   = ((*y   >> 4) * 10) + (*y   & 0xF);
		*m   = ((*m   >> 4) * 10) + (*m   & 0xF);
		*d   = ((*d   >> 4) * 10) + (*d   & 0xF);
		*hr  = ((*hr  >> 4) * 10) + (*hr  & 0xF);
		*min = ((*min >> 4) * 10) + (*min & 0xF);
		*sec = ((*sec >> 4) * 10) + (*sec & 0xF);
	}
}
