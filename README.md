# SegTerm

A VT100-compatible serial terminal (among other applications) built out of 72 individual 4-digit 7-segment display modules.

Sometimes even I have to ask myself, "why!?"

There are however actually useful things to come out of this:

### KeyboardLib

An Arduino library for driving an 8x8 keyboard matrix. Accompanied by PCB designs for a QWERTY keyboard with arrow keys and a 64-key piano keyboard.

* [Keyboard demo sketch](KeyboardDemo/)
* [QWERTY keyboard PCB files](board/TacText/)
* [Piano keyboard PCB files](board/TacTune/)
* [Order the QWERTY keyboard PCB](https://aisler.net/p/XQUBPOEX) from Aisler
* [Order the piano keyboard PCB](https://aisler.net/p/HVOZQIGT) from Aisler

### PlayduinoLib

An Arduino library for playing tunes using *scripted music notation*. Scripted music notation uses a sequence of note strings separated by whitespace to represent a sequence of notes. Each note string is composed of a pitch followed by a duration or a duration followed by the letter `r` to represent a rest.

The pitch is specified as a note name (`C`, `D`, `E`, `F`, `G`, `A`, `B`), followed by zero or more `b` or `#` symbols (representing flats and sharps respectively), optionally followed by an octave number (`00`, `0`, `1`, `2`, `3`, `4`, `5`, `6`, `7`, `8`, `9`), where octave 4 is the octave starting with middle C and containing A440. If the pitch or octave is not specified, the previous value is used.

The duration is specified as one of the following letters, followed by zero or more `:` symbols (which double the note value), `.` symbols (to create a dotted note), or digits `1` through `9` (to create a tuplet note). (Note that a digit will create an individual note in a tuplet, not the whole tuplet.) If the duration is not specified, the previous value is used.

|     |                      |     |              |     |            |
| --- | -------------------- | --- | ------------ | --- | ---------- |
| `z` | octuple whole note   | `w` | whole note   | `s` | 16th note  |
| `l` | sextuple whole note  | `h` | half note    | `t` | 32nd note  |
| `i` | quadruple whole note | `q` | quarter note | `x` | 64th note  |
| `u` | double whole note    | `e` | eighth note  | `o` | 128th note |

A tune can be prefixed with a `%` followed by a number to indicate the tempo.

Here is a very simple sample sketch:

```
#include <Arduino.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include "PlayduinoLib.h"

#define PIEZO_PIN 8

void setup() {
  play(PIEZO_PIN, "%200 C4e C Dq C F Eh")
}

void loop() {
}
```

* [Playduino demo sketch](Playduino/)

### BusyBoard Mega

A shield for the Arduino Mega providing connections for a number of peripherals:

* [Real-Time Clock](https://www.sparkfun.com/products/12708)
* [Audio input](https://aisler.net/p/ZNJUIHOZ)
* [Piezo speaker](https://www.adafruit.com/product/160)
* [Vibration sensor](https://www.adafruit.com/product/1766)
* [NeoPixel](https://www.adafruit.com/product/1312)
* Three [tactile switches](https://www.sparkfun.com/products/97)
* 8x8 keyboard matrix (see above)

This is designed to be used with the keyboards mentioned above, but of course you can also use it just for the other components.

* [BusyBoard Mega PCB files](board/BusyBoardMega/)
* [Order a BusyBoard Mega](https://aisler.net/p/KVFFFSNR) from Aisler
