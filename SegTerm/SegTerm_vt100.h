#ifndef __SEGTERM_VT100_H__
#define __SEGTERM_VT100_H__

#include <inttypes.h>

#define VT100_MODE_VT52          0x80
#define VT100_MODE_INSERT        0x40
#define VT100_MODE_NEWLINE       0x20
#define VT100_MODE_DEFAULT       0x06
#define VT100_MODE_CURSOR_ON     0x04
#define VT100_MODE_CURSOR_BLINK  0x02
#define VT100_MODE_CURSOR_ACTIVE 0x01

#define VT100_STATE_BASE            0
#define VT100_STATE_ESC             1
#define VT100_STATE_VT52_DCA        2
#define VT100_STATE_DCS             3
#define VT100_STATE_SOS             4
#define VT100_STATE_CSI             5
#define VT100_STATE_CSI_PRIVATE     6
#define VT100_STATE_OSC             7
#define VT100_STATE_PM              8
#define VT100_STATE_APC             9

#define VT100_MAX_PARAMS           16
#define VT100_OUTBUF_SIZE          16

#define VT100_ATTR_BRIGHT        0x01
#define VT100_ATTR_DIM           0x02
#define VT100_ATTR_ITALIC        0x04
#define VT100_ATTR_UNDERLINE     0x08
#define VT100_ATTR_BLINK         0x10
#define VT100_ATTR_FLASH         0x20
#define VT100_ATTR_REVERSE       0x40
#define VT100_ATTR_HIDDEN        0x80

#define VT100_COLOR_BLACK           0
#define VT100_COLOR_RED             1
#define VT100_COLOR_GREEN           2
#define VT100_COLOR_YELLOW          3
#define VT100_COLOR_BLUE            4
#define VT100_COLOR_MAGENTA         5
#define VT100_COLOR_CYAN            6
#define VT100_COLOR_WHITE           7
#define VT100_COLOR_BRIGHT          8

#define VT100_BRIGHTNESS_BRIGHT    15
#define VT100_BRIGHTNESS_NORMAL    10
#define VT100_BRIGHTNESS_DIM        5

#define VT100_BRIGHTNESS_BLACK      0
#define VT100_BRIGHTNESS_RED        4
#define VT100_BRIGHTNESS_GREEN      9
#define VT100_BRIGHTNESS_YELLOW    13
#define VT100_BRIGHTNESS_BLUE       2
#define VT100_BRIGHTNESS_MAGENTA    6
#define VT100_BRIGHTNESS_CYAN      11
#define VT100_BRIGHTNESS_WHITE     15

#define VT100_CURSOR_BLINK_TIME   441

void    initVT100();

uint8_t vtGetCursorY();
uint8_t vtGetCursorX();
uint8_t vtGetAttributes();
uint8_t vtGetTextColor();
void    vtSetCursor(uint8_t y, uint8_t x);
void    vtSetCursorY(uint8_t y);
void    vtSetCursorX(uint8_t x);
void    vtSetAttrAndColor(uint8_t attr, uint8_t color);
void    vtSetAttributes(uint8_t attr);
void    vtSetTextColor(uint8_t color);

void    vtPutChar(uint8_t ch);
void    vtPrint(const char * s);
void    vtPrintln(const char * s);
uint8_t vtGetChar();
void    vtIdle();

#endif
