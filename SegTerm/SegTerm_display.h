#ifndef __SEGTERM_DISPLAY_H__
#define __SEGTERM_DISPLAY_H__

#include <inttypes.h>

#define HT16K33_BASE_ADDRESS      0xE0
#define HT16K33_START_OSCILLATOR  0x21
#define HT16K33_BLINK_CMD         0x80
#define HT16K33_BLINK_DISPLAYON   0x01
#define HT16K33_BLINK_OFF            0
#define HT16K33_BLINK_2HZ            1
#define HT16K33_BLINK_1HZ            2
#define HT16K33_BLINK_HALFHZ         3
#define HT16K33_CMD_BRIGHTNESS    0xE0

#define SEGTERM_FONT_READABLE        0
#define SEGTERM_FONT_DISTINCT        1

#define SEGTERM_CHATTR_HIDDEN     0x08
#define SEGTERM_CHATTR_RAW        0x04
#define SEGTERM_CHATTR_XORMASK    0x02
#define SEGTERM_CHATTR_DOT        0x01

#define SEGTERM_LOWERCASE_OFF        0
#define SEGTERM_LOWERCASE_ON         1

#define SEGTERM_DOTMODE_HIGHBIT   0x80
#define SEGTERM_DOTMODE_LETTER    0x03
#define SEGTERM_DOTMODE_UPPERCASE 0x02
#define SEGTERM_DOTMODE_LOWERCASE 0x01

#define SEGTERM_XORMASK_DEFAULT   0x88

void    initDisplay();
void    loadFont(uint8_t font);
void    loadFontFromEEPROM(int addr);
void    saveFontToEEPROM(int addr);
void    loadDefaultDisplaySettings();
void    loadDisplaySettingsFromEEPROM(int addr);
void    saveDisplaySettingsToEEPROM(int addr);
uint8_t getFontChar(uint8_t ch);
void    setFontChar(uint8_t ch, uint8_t data);
uint8_t getChar(uint8_t row, uint8_t col);
void    setChar(uint8_t row, uint8_t col, uint8_t ch);
uint8_t getChAttr(uint8_t row, uint8_t col);
void    setChAttr(uint8_t row, uint8_t col, uint8_t a);
void    setCharAndAttr(uint8_t row, uint8_t col, uint8_t ch, uint8_t a);
uint8_t getMdAttr(uint8_t row, uint8_t col);
void    setMdAttr(uint8_t row, uint8_t col, uint8_t a);
uint8_t getMdBright(uint8_t row, uint8_t col);
void    setMdBright(uint8_t row, uint8_t col, uint8_t b);
uint8_t getMdBlink(uint8_t row, uint8_t col);
void    setMdBlink(uint8_t row, uint8_t col, uint8_t b);
uint8_t getMdColon(uint8_t row, uint8_t col);
void    setMdColon(uint8_t row, uint8_t col, uint8_t c);
uint8_t getLcMode();
void    setLcMode(uint8_t lc);
uint8_t getDotMode();
void    setDotMode(uint8_t d);
uint8_t getBrightness();
void    setBrightness(uint8_t b);
uint8_t getXorMask();
void    setXorMask(uint8_t m);
uint8_t lockScreen();
uint8_t unlockScreen();
void    scrollRows(uint8_t top, uint8_t bottom, int8_t dir);
void    scrollCols(uint8_t row, uint8_t left, uint8_t right, int8_t dir);
void    clearRows(uint8_t row1, uint8_t row2);
void    clearCols(uint8_t row, uint8_t col1, uint8_t col2);

#endif
