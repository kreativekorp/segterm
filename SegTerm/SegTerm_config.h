#ifndef __SEGTERM_CONFIG_H__
#define __SEGTERM_CONFIG_H__

/* The number of display modules per row.    *
 * Each module has four digits and a colon.  *
 * The maximum number is 8 which corresponds *
 * to the number of possible I2C addresses   *
 * for an Adafruit 7-segment LED backpack.   */
#define SEGTERM_COLS  6

/* The number of rows. The maximum number is 16 *
 * which corresponds to half the number of pins *
 * on the far right side of the Arduino Mega.   */
#define SEGTERM_ROWS 12

#endif
