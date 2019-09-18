#ifndef __SEGTERM_EEPROM_H__
#define __SEGTERM_EEPROM_H__

#include <inttypes.h>

#define EE_CHECK1      0
#define EE_CHECK2      1
#define EE_LCMODE      2
#define EE_DOTMODE     3
#define EE_BRIGHTNESS  4
#define EE_XORMASK     5
#define EE_CHECK3      6
#define EE_VTMODE      7
#define EE_CENTURY     8
#define EE_FIRSTDAY    9
#define EE_CURRAPP    10
#define EE_PIANOMODE  11
#define EE_FONT       16

#define EE_CHECK1_VALUE 0x57
#define EE_CHECK2_VALUE 0xEE
#define EE_CHECK3_VALUE 0x87

void EEPROMwrite(const int addr, const uint8_t val);

#endif
