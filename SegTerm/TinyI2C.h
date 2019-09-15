#ifndef __TINYI2C_H__
#define __TINYI2C_H__

#include <Arduino.h>
#include <inttypes.h>

#define I2C_READ  1
#define I2C_WRITE 0
#define I2C_DELAY 4

class TinyI2C {
	public:
		TinyI2C(
			volatile unsigned char *sda_ddr,
			volatile unsigned char *sda_port,
			volatile unsigned char *sda_pin,
			uint8_t                sda_mask,
			volatile unsigned char *scl_ddr,
			volatile unsigned char *scl_port,
			volatile unsigned char *scl_pin,
			uint8_t                scl_mask
		);

		uint8_t init();
		uint8_t start(uint8_t addr);
		uint8_t restart(uint8_t addr);
		uint8_t write(uint8_t value);
		uint8_t read(uint8_t last);
		void    stop();

	private:
		volatile unsigned char *_sda_ddr;
		volatile unsigned char *_sda_port;
		volatile unsigned char *_sda_pin;
		uint8_t                _sda_mask;
		volatile unsigned char *_scl_ddr;
		volatile unsigned char *_scl_port;
		volatile unsigned char *_scl_pin;
		uint8_t                _scl_mask;
};

#endif
