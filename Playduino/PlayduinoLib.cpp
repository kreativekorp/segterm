#include <Arduino.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include "PlayduinoLib.h"

static const unsigned int NOTE_FREQ[] PROGMEM = {
	8,      9,      9,      10,     10,     11,     12,     12,
	13,     14,     15,     15,     16,     17,     18,     19,
	21,     22,     23,     24,     26,     28,     29,     31,
	33,     35,     37,     39,     41,     44,     46,     49,
	52,     55,     58,     62,     65,     69,     73,     78,
	82,     87,     92,     98,     104,    110,    117,    123,
	131,    139,    147,    156,    165,    175,    185,    196,
	208,    220,    233,    247,    262,    277,    294,    311,
	330,    349,    370,    392,    415,    440,    466,    494,
	523,    554,    587,    622,    659,    698,    740,    784,
	831,    880,    932,    988,    1047,   1109,   1175,   1245,
	1319,   1397,   1480,   1568,   1661,   1760,   1865,   1976,
	2093,   2217,   2349,   2489,   2637,   2794,   2960,   3136,
	3322,   3520,   3729,   3951,   4186,   4435,   4699,   4978,
	5274,   5588,   5920,   6272,   6645,   7040,   7459,   7902,
	8372,   8870,   9397,   9956,   10548,  11175,  11840,  12544
};

unsigned int getNoteFrequency(uint8_t p) {
	return pgm_read_word(&NOTE_FREQ[p & 0x7F]);
}

void play(uint8_t pin, const char * notes) {
	unsigned long tempo = 120; // default tempo in beats per minute
	unsigned int pitch = 12; // 12 higher than actual pitch to accommodate accidentals
	unsigned int octave = 5; // 1 higher than actual octave to accommodate octave 00
	unsigned long duration = 60000; // duration of a quarter note in millis at 1 bpm
	unsigned long lastStartTime = 0;
	unsigned long lastDuration = 0;
	unsigned long toneDuration;
	unsigned int tonePitch;
	unsigned int toneFreq;
	const char * end;
	char rest;
	char ch;
	for (;;) {
		// Skip whitespace and look for tempo changes
		for (;;) {
			ch = *notes;
			if (!ch) {
				// End of string
				// Wait for last note to finish (if there was one)
				while ((millis() - lastStartTime) < lastDuration);
				return;
			} else if (ch == '%') {
				// Tempo change
				++notes;
				tempo = 0;
				for (;;) {
					ch = *notes;
					if (ch < '0' || ch > '9') break;
					tempo *= 10;
					tempo += ch - '0';
					++notes;
				}
				if (!tempo) tempo = 120;
			} else if (ch <= 0x20 || ch >= 0x7F) {
				// Whitespace
				++notes;
			} else {
				// Start of note string
				break;
			}
		}

		// Find end of note string and check for rest
		rest = 0;
		end = notes;
		for (;;) {
			ch = *end;
			if (ch == 'R' || ch == 'r') rest = 1;
			if (ch > 0x20 && ch < 0x7F) ++end;
			else break;
		}

		// Parse pitch
		if (!rest) do {
			switch (*notes) {
				// 12 higher than actual pitch to accommodate accidentals
				case 'C': case 'c': pitch = 12; break;
				case 'D': case 'd': pitch = 14; break;
				case 'E': case 'e': pitch = 16; break;
				case 'F': case 'f': pitch = 17; break;
				case 'G': case 'g': pitch = 19; break;
				case 'A': case 'a': pitch = 21; break;
				case 'B': case 'b': pitch = 23; break;
				case 'M': case 'm': case 'N': case 'n':
				case 'P': case 'p': case '$': pitch = 0; break;
				default: continue; // skip this whole section
			}
			++notes;
			if (pitch) {
				for (;;) {
					ch = *notes;
					if (ch == 'B' || ch == 'b') {
						if (pitch > 0) --pitch;
						++notes;
					} else if (ch == '#') {
						if (pitch < 36) ++pitch;
						++notes;
					} else {
						break;
					}
				}
				if (ch >= '0' && ch <= '9') {
					octave = ch - '0' + 1;
					++notes;
				}
				if (ch == '0' && (*notes) == '0') {
					octave = 0;
					++notes;
				}
			} else {
				for (;;) {
					ch = *notes;
					if (ch < '0' || ch > '9') break;
					pitch *= 10;
					pitch += ch - '0';
					++notes;
				}
				octave = pitch / 12;
				pitch %= 12;
				pitch += 12;
			}
		} while (0);

		// Parse duration
		do {
			switch (*notes) {
				// Durations are given in millis at 1 bpm
				case 'Z': case 'z': duration = 1920000; break; // 8x whole note
				case 'L': case 'l': duration = 1440000; break; // 6x whole note
				case 'I': case 'i': duration = 960000; break; // 4x whole note
				case 'U': case 'u': duration = 480000; break; // 2x whole note
				case 'W': case 'w': duration = 240000; break; // whole note
				case 'H': case 'h': duration = 120000; break; // half note
				case 'Q': case 'q': duration = 60000; break; // quarter note
				case 'E': case 'e': duration = 30000; break; // eighth note
				case 'S': case 's': duration = 15000; break; // sixteenth note
				case 'T': case 't': duration = 7500; break; // 32nd note
				case 'X': case 'x': duration = 3750; break; // 64th note
				case 'O': case 'o': duration = 1875; break; // 128th note
				case 'D': case 'd': duration = 0; break; // custom
				default: continue; // skip this whole section
			}
			++notes;
			if (duration) {
				for (;;) {
					ch = *notes;
					if (ch >= '1' && ch <= '9') duration /= ch - '0';
					else if (ch == '.') duration += duration / 2;
					else if (ch == ':') duration *= 2;
					else break;
					++notes;
				}
			} else {
				for (;;) {
					ch = *notes;
					if (ch < '0' || ch > '9') break;
					duration *= 10;
					duration += ch - '0';
					++notes;
				}
				duration *= 1875;
				duration /= 2;
			}
		} while (0);

		// Wait for previous note to finish (if there was one)
		while ((millis() - lastStartTime) < lastDuration);

		// Play the tone
		lastStartTime = millis();
		lastDuration = duration / tempo;
		if (!rest) {
			toneDuration = lastDuration;
			if ((*notes) == ',') toneDuration /= 2;
			if (toneDuration > PLAYDUINO_NOTE_DELAY) {
				toneDuration -= PLAYDUINO_NOTE_DELAY;
				tonePitch = octave * 12 + pitch - 12;
				if (tonePitch > 0 && tonePitch < 128) {
					toneFreq = pgm_read_word(&NOTE_FREQ[tonePitch]);
					tone(pin, toneFreq, toneDuration);
				}
			}
		}

		// Skip to end of note string
		notes = end;
	}
}
