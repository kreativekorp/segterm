#include <Arduino.h>
#include <inttypes.h>
#include "TinyI2C.h"

TinyI2C::TinyI2C(
	volatile unsigned char *sda_ddr,
	volatile unsigned char *sda_port,
	volatile unsigned char *sda_pin,
	uint8_t                sda_mask,
	volatile unsigned char *scl_ddr,
	volatile unsigned char *scl_port,
	volatile unsigned char *scl_pin,
	uint8_t                scl_mask
) {
	_sda_ddr  = sda_ddr;
	_sda_port = sda_port;
	_sda_pin  = sda_pin;
	_sda_mask = sda_mask;
	_scl_ddr  = scl_ddr;
	_scl_port = scl_port;
	_scl_pin  = scl_pin;
	_scl_mask = scl_mask;
}

#define _sdaHigh() do { noInterrupts(); *_sda_ddr &=~ _sda_mask; *_sda_port |=  _sda_mask; interrupts(); } while(0)
#define _sdaLow()  do { noInterrupts(); *_sda_ddr |=  _sda_mask; *_sda_port &=~ _sda_mask; interrupts(); } while(0)
#define _sclHigh() do { noInterrupts(); *_scl_ddr &=~ _scl_mask; *_scl_port |=  _scl_mask; interrupts(); } while(0)
#define _sclLow()  do { noInterrupts(); *_scl_ddr |=  _scl_mask; *_scl_port &=~ _scl_mask; interrupts(); } while(0)
#define _sdaRead() (*_sda_pin & _sda_mask)
#define _sclRead() (*_scl_pin & _scl_mask)

uint8_t TinyI2C::init() {
	_sdaLow();  _sclLow();
	_sdaHigh(); _sclHigh();
	return _sdaRead() && _sclRead();
}

uint8_t TinyI2C::start(uint8_t addr) {
	_sdaLow();
	_sclLow();
	return write(addr);
}

uint8_t TinyI2C::restart(uint8_t addr) {
	_sdaHigh();
	_sclHigh();
	return start(addr);
}

uint8_t TinyI2C::write(uint8_t value) {
	uint8_t m;
	for (m = 0x80; m; m >>= 1) {
		if (value & m) _sdaHigh();
		else           _sdaLow();
		_sclHigh();
		_sclLow();
	}
	_sdaHigh();
	_sclHigh();
	m = _sdaRead();
	_sclLow();
	_sdaLow();
	return !m;
}

uint8_t TinyI2C::read(uint8_t last) {
	uint8_t b = 0, i;
	_sdaHigh();
	for (i = 0; i < 8; ++i) {
		b <<= 1;
		_sclHigh();
		if (_sdaRead()) b |= 1;
		_sclLow();
	}
	if (last) _sdaHigh();
	else      _sdaLow();
	_sclHigh();
	_sclLow();
	_sdaLow();
	return b;
}

void TinyI2C::stop() {
	_sdaLow();
	_sclHigh();
	_sdaHigh();
}

#undef _sdaHigh
#undef _sdaLow
#undef _sclHigh
#undef _sclLow
#undef _sdaRead
#undef _sclRead
