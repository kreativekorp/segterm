#ifndef __KEYBOARD_LIB_H__
#define __KEYBOARD_LIB_H__

#include <inttypes.h>

// Arduino Mega D15
#define KBD_LED_DDR  DDRJ
#define KBD_LED_PORT PORTJ
#define KBD_LED_PIN  PINJ
#define KBD_LED_MASK 0x01

// Arduino Mega D16
#define KBD_ANODE_0_DDR  DDRH
#define KBD_ANODE_0_PORT PORTH
#define KBD_ANODE_0_PIN  PINH
#define KBD_ANODE_0_MASK 0x02

// Arduino Mega D17
#define KBD_ANODE_1_DDR  DDRH
#define KBD_ANODE_1_PORT PORTH
#define KBD_ANODE_1_PIN  PINH
#define KBD_ANODE_1_MASK 0x01

// Arduino Mega D18
#define KBD_ANODE_2_DDR  DDRD
#define KBD_ANODE_2_PORT PORTD
#define KBD_ANODE_2_PIN  PIND
#define KBD_ANODE_2_MASK 0x08

// Arduino Mega D19
#define KBD_ANODE_3_DDR  DDRD
#define KBD_ANODE_3_PORT PORTD
#define KBD_ANODE_3_PIN  PIND
#define KBD_ANODE_3_MASK 0x04

// Arduino Mega A4
#define KBD_ANODE_4_DDR  DDRF
#define KBD_ANODE_4_PORT PORTF
#define KBD_ANODE_4_PIN  PINF
#define KBD_ANODE_4_MASK 0x10

// Arduino Mega A5
#define KBD_ANODE_5_DDR  DDRF
#define KBD_ANODE_5_PORT PORTF
#define KBD_ANODE_5_PIN  PINF
#define KBD_ANODE_5_MASK 0x20

// Arduino Mega A6
#define KBD_ANODE_6_DDR  DDRF
#define KBD_ANODE_6_PORT PORTF
#define KBD_ANODE_6_PIN  PINF
#define KBD_ANODE_6_MASK 0x40

// Arduino Mega A7
#define KBD_ANODE_7_DDR  DDRF
#define KBD_ANODE_7_PORT PORTF
#define KBD_ANODE_7_PIN  PINF
#define KBD_ANODE_7_MASK 0x80

// Arduino Mega A8
#define KBD_CATHODE_0_DDR  DDRK
#define KBD_CATHODE_0_PORT PORTK
#define KBD_CATHODE_0_PIN  PINK
#define KBD_CATHODE_0_MASK 0x01

// Arduino Mega A9
#define KBD_CATHODE_1_DDR  DDRK
#define KBD_CATHODE_1_PORT PORTK
#define KBD_CATHODE_1_PIN  PINK
#define KBD_CATHODE_1_MASK 0x02

// Arduino Mega A10
#define KBD_CATHODE_2_DDR  DDRK
#define KBD_CATHODE_2_PORT PORTK
#define KBD_CATHODE_2_PIN  PINK
#define KBD_CATHODE_2_MASK 0x04

// Arduino Mega A11
#define KBD_CATHODE_3_DDR  DDRK
#define KBD_CATHODE_3_PORT PORTK
#define KBD_CATHODE_3_PIN  PINK
#define KBD_CATHODE_3_MASK 0x08

// Arduino Mega A12
#define KBD_CATHODE_4_DDR  DDRK
#define KBD_CATHODE_4_PORT PORTK
#define KBD_CATHODE_4_PIN  PINK
#define KBD_CATHODE_4_MASK 0x10

// Arduino Mega A13
#define KBD_CATHODE_5_DDR  DDRK
#define KBD_CATHODE_5_PORT PORTK
#define KBD_CATHODE_5_PIN  PINK
#define KBD_CATHODE_5_MASK 0x20

// Arduino Mega A14
#define KBD_CATHODE_6_DDR  DDRK
#define KBD_CATHODE_6_PORT PORTK
#define KBD_CATHODE_6_PIN  PINK
#define KBD_CATHODE_6_MASK 0x40

// Arduino Mega A15
#define KBD_CATHODE_7_DDR  DDRK
#define KBD_CATHODE_7_PORT PORTK
#define KBD_CATHODE_7_PIN  PINK
#define KBD_CATHODE_7_MASK 0x80

#define KBD_PROP_DELAY      16
#define KBD_DEBOUNCE_DELAY 100
#define KBD_BUFFER_SIZE    128

#define KBD_PRESSED  0x80
#define KBD_RELEASED 0x40
#define KBD_KEY      0x3F

void    initKeyboard();
void    readKeyboardState(uint8_t * state);
uint8_t readKeyboardEvent();

#endif
