#ifndef __SEGTERM_EEPROM_H__
#define __SEGTERM_EEPROM_H__

#include <inttypes.h>

#define EE_CHECK1      0
#define EE_CHECK2      1
#define EE_LCMODE      2
#define EE_DOTMODE     3
#define EE_BRIGHTNESS  4
#define EE_XORMASK     5
#define EE_FONT       16

void EEPROMwrite(const int addr, const uint8_t val);

#endif
