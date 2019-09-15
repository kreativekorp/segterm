#ifndef __KEYBOARD_LIB_H__
#define __KEYBOARD_LIB_H__

#include <Arduino.h>
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

#define KBD_KEY_6      0x00
#define KBD_KEY_5      0x01
#define KBD_KEY_4      0x02
#define KBD_KEY_3      0x03
#define KBD_KEY_2      0x04
#define KBD_KEY_1      0x05
#define KBD_KEY_ESC    0x06
#define KBD_KEY_LCTRL  0x07
#define KBD_KEY_Y      0x08
#define KBD_KEY_T      0x09
#define KBD_KEY_R      0x0A
#define KBD_KEY_E      0x0B
#define KBD_KEY_W      0x0C
#define KBD_KEY_Q      0x0D
#define KBD_KEY_TAB    0x0E
#define KBD_KEY_LALT   0x0F
#define KBD_KEY_H      0x10
#define KBD_KEY_G      0x11
#define KBD_KEY_F      0x12
#define KBD_KEY_D      0x13
#define KBD_KEY_S      0x14
#define KBD_KEY_A      0x15
#define KBD_KEY_CAPS   0x16
#define KBD_KEY_SPACE  0x17
#define KBD_KEY_N      0x18
#define KBD_KEY_B      0x19
#define KBD_KEY_V      0x1A
#define KBD_KEY_C      0x1B
#define KBD_KEY_X      0x1C
#define KBD_KEY_Z      0x1D
#define KBD_KEY_ANGLE  0x1E
#define KBD_KEY_RALT   0x1F
#define KBD_KEY_M      0x20
#define KBD_KEY_COMMA  0x21
#define KBD_KEY_PERIOD 0x22
#define KBD_KEY_SLASH  0x23
#define KBD_KEY_SHIFT  0x24
#define KBD_KEY_LEFT   0x25
#define KBD_KEY_RIGHT  0x26
#define KBD_KEY_RCTRL  0x27
#define KBD_KEY_J      0x28
#define KBD_KEY_K      0x29
#define KBD_KEY_L      0x2A
#define KBD_KEY_COLON  0x2B
#define KBD_KEY_QUOTE  0x2C
#define KBD_KEY_ENTER  0x2D
#define KBD_KEY_UP     0x2E
#define KBD_KEY_DOWN   0x2F
#define KBD_KEY_U      0x30
#define KBD_KEY_I      0x31
#define KBD_KEY_O      0x32
#define KBD_KEY_P      0x33
#define KBD_KEY_LBRACK 0x34
#define KBD_KEY_RBRACK 0x35
#define KBD_KEY_BSLASH 0x36
#define KBD_KEY_TILDE  0x37
#define KBD_KEY_7      0x38
#define KBD_KEY_8      0x39
#define KBD_KEY_9      0x3A
#define KBD_KEY_0      0x3B
#define KBD_KEY_HYPHEN 0x3C
#define KBD_KEY_EQUAL  0x3D
#define KBD_KEY_BKSP   0x3E
#define KBD_KEY_DEL    0x3F

#define KBD_MOD_CAPS   0x80
#define KBD_MOD_SHIFT  0x40
#define KBD_MOD_LCTRL  0x20
#define KBD_MOD_LALT   0x10
#define KBD_MOD_RCTRL  0x02
#define KBD_MOD_RALT   0x01
#define KBD_MOD_CTRL   0x22
#define KBD_MOD_ALT    0x11

#define KBD_ASCII_BKSP    8
#define KBD_ASCII_TAB     9
#define KBD_ASCII_ENTER  10
#define KBD_ASCII_SHIFT  16
#define KBD_ASCII_CTRL   17
#define KBD_ASCII_ALT    18
#define KBD_ASCII_CAPS   20
#define KBD_ASCII_ESC    27
#define KBD_ASCII_LEFT   28
#define KBD_ASCII_RIGHT  29
#define KBD_ASCII_UP     30
#define KBD_ASCII_DOWN   31
#define KBD_ASCII_SPACE  32
#define KBD_ASCII_DEL   127

void    initKeyboard();
void    readKeyboardState(uint8_t * state);
uint8_t readKeyboardEvent();
uint8_t getKeyboardModifiers();
void    setKeyboardModifiers(uint8_t mod);
void    setKeyboardLED(uint8_t led);
char    keyboardEventToASCII(uint8_t evt, uint8_t mod);

#endif
