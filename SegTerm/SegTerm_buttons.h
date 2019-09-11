#ifndef __SEGTERM_BUTTONS_H__
#define __SEGTERM_BUTTONS_H__

#include <Arduino.h>
#include <inttypes.h>

// Arduino Mega A1
#define BUTTON_1_DDR  DDRF
#define BUTTON_1_PORT PORTF
#define BUTTON_1_PIN  PINF
#define BUTTON_1_MASK 0x02

// Arduino Mega A2
#define BUTTON_2_DDR  DDRF
#define BUTTON_2_PORT PORTF
#define BUTTON_2_PIN  PINF
#define BUTTON_2_MASK 0x04

// Arduino Mega A3
#define BUTTON_3_DDR  DDRF
#define BUTTON_3_PORT PORTF
#define BUTTON_3_PIN  PINF
#define BUTTON_3_MASK 0x08

#define BUTTON_1 0x01
#define BUTTON_2 0x02
#define BUTTON_3 0x04

void    initButtons();
uint8_t getButtons();

#endif
