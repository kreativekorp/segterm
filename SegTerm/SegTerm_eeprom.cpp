#include <EEPROM.h>
#include <inttypes.h>
#include "SegTerm_eeprom.h"

void EEPROMwrite(const int addr, const uint8_t val) {
	if (EEPROM.read(addr) != val) {
		EEPROM.write(addr, val);
	}
}
