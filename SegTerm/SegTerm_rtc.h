#ifndef __SEGTERM_RTC_H__
#define __SEGTERM_RTC_H__

#include <inttypes.h>

#define RTC_ADDR 0x68

#define MIL 0
#define AM  1
#define PM  2

uint8_t daysInMonth(
	uint8_t c, uint8_t y, uint8_t m,
	uint8_t bcd
);

uint8_t dayOfWeek(
	uint8_t c, uint8_t y, uint8_t m, uint8_t d,
	uint8_t bcd
);

void setTime(
	uint8_t c, uint8_t y, uint8_t m, uint8_t d,
	uint8_t hr, uint8_t min, uint8_t sec, uint8_t ampm,
	uint8_t bcd
);

void getTime(
	uint8_t * c, uint8_t * y, uint8_t * m, uint8_t * d, uint8_t * w,
	uint8_t * hr, uint8_t * min, uint8_t * sec, uint8_t * ampm,
	uint8_t bcd
);

#endif
