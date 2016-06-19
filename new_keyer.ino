#include <Arduino.h>

#include "options.h"

#include "keying.h"
#include "display.h"

#define LEFT_PADDLE 5
#define RIGHT_PADDLE 2

#define PTT_1    13
#define KEY_OUT_1 11
#define SIDETONE_FREQUENCY 800

#if defined(DISPLAY_LARGE)
String xmit_strings[] = {
    "Xmitter 1",
    "Xmitter 2",
    "Xmitter 3",
    "Xmitter 4",
    "Practice "
};
#else // !defined(DISPLAY_LARGE)
String key_modestrings[] = {
    "Tx1",
    "Tx2",
    "Tx3",
    "Tx4",
    "CPO"
};
#endif // !defined(DISPLAY_LARGE)

#define MIN_WPM 5L
#define MAX_WPM 40L
#define MS_PER_DOT 1200
#define MS_PER_DASH (3*MS_PER_DOT)
#define MS_PER_WORD (7*MS_PER_DOT)

int potPin = A0;
int potValue = 0;

unsigned dot_twitches;
unsigned dash_twitches;
unsigned word_twitches;
unsigned long last_ms;
unsigned long next_state_transition_ms;

#if defined(DISPLAY_LARGE)
String key_modestrings[] = {
    "Iambic Mode A",
    "Iambic Mode B",
    "Ultimatic"
    "Semiautomatic",
    "Straight"
};
#else // !defined(DISPLAY_LARGE)
String key_modestrings[] = {
"IamA",
    "IamB",
    "Ulti"
    "Bug ",
    "Strt"
    };
#endif // !defined(DISPLAY_LARGE)

typedef enum keyer_state { KEY_DIT, KEY_DAH, KEY_UP } keyer_state_t;

keyer_state_t keyer_state;
keyer_state_t next_keyer_state;

keying transmitter(PTT_1, KEY_OUT_1, SIDETONE_FREQUENCY);
display display_manager;

unsigned find_wpm() {
    unsigned wpm;
    // The WPM is MIN_WPM + (MAX_WPM - MIN_WPM) * 1024 / potValue;
    potValue = analogRead(potPin);
    wpm = MIN_WPM + ((MAX_WPM - MIN_WPM) * potValue + 512L) / 1024L;
    display_manager.wpm(wpm);
    return wpm;
}

typedef enum keyer_mode { MODE_PADDLE_NORMAL, MODE_PADDLE_REVERSE, MODE_KEYBOARD, MODE_SERIAL, MODE_MEMORY } keyer_mode_t;

keyer_mode_t keyer_mode;
uint8_t kbd_bit, kbd_count;

#if defined(DISPLAY_LARGE)
String input_strings[] = {
    "Paddles Normal",
    "Paddles Reverse",
    "Keyboard",
    "Serial Port",
    "Memory"
};
#else // !defined(DISPLAY_LARGE)
String input_strings[] = {
    "Nor",
    "Rev",
    "Kbd",
    "Srl",
    "Mem"
};
#endif // !defined(DISPLAY_LARGE)

// The most significant four bits holds a count of the sounds made
// The bottom 12 bits hold a bitmap of the length of the sounds, with the
// least significant bit holding the first sound, the next least significant
// bit holding the second sound, and so forth.  If the sound is long (a DAH)
// then the corresponding bit is a one, otherwise the bit is a zero.
// Eventually, the values, 0, 13, 14, and 15 for the most significant four
// bits will mean different things.
uint16_t morse_table[] = {
 // 000  001 002 003 004 005 006 007
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 // 010     011     012     013     014     015     016     017
    0x0000, 0xd000, 0xd000, 0xd000, 0xd000, 0xd000, 0x0000, 0x0000,
 // 020 021 022 023 024 025 026 027
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 // 030 031 032 033 034 035 036 037
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 // 040 ' ' 041 '!' 042 '"' 043 '#' 044 '$' 045 '%' 046 '&' 047 '''
    0xd000, 0x6035, 0x6012, 0x0000, 0x7048, 0x0000, 0x5002, 0x601e,
 // 050 '(' 051 ')' 052 '*' 053 '+' 054 ',' 055 '-' 056 '.' 057 '/'
    0x500d, 0x602d, 0x0000, 0x500a, 0x6033, 0x6021, 0x602a, 0x5009,
 // 060 '0' 061 '1' 062 '2' 063 '3' 064 '4' 065 '5' 066 '6' 067 '7'
    0x501f, 0x501e, 0x501c, 0x5018, 0x5010, 0x5000, 0x5001, 0x5003,
 // 070 '8' 071 '9' 072 ':' 073 ';' 074 '<' 075 '=' 076 '>' 077 '?'
    0x5007, 0x500f, 0x6007, 0x6015, 0x0000, 0x5011, 0x0000, 0x600c,

 // 100 '@' 101 'A' 102 'B' 103 'C' 104 'D' 105 'E' 106 'F' 107 'G'
    0x6016, 0x2002, 0x4001, 0x4005, 0x3001, 0x1000, 0x4004, 0x3003,
 // 110 'H' 111 'I' 112 'J' 113 'K' 114 'L' 115 'M' 116 'N' 117 'O'
    0x4000, 0x2000, 0x400e, 0x3005, 0x4002, 0x2003, 0x2001, 0x3007,
 // 120 'P' 121 'Q' 122 'R' 123 'S' 124 'T' 125 'U' 126 'V' 127 'W'
    0x4006, 0x400b, 0x3002, 0x3000, 0x1001, 0x3004, 0x4008, 0x3006,
 // 130 'X' 131 'Y' 132 'Z' 133 '[' 134 '\' 135 ']' 136 '^' 137 '_'
    0x4009, 0x400d, 0x4003, 0x0000, 0x0000, 0x0000, 0x0000, 0x602c,
 // 140 '`' 141 'a' 142 143 144 145 146 147
    0x0000, 0x2002, 0x4001, 0x4005, 0x3001, 0x1000, 0x4004, 0x3003,
    // 150 151 152 153 154 155 156 157
    0x4000, 0x2000, 0x400e, 0x3005, 0x4002, 0x2003, 0x2001, 0x3007,
    // 160 161 162 163 164 165 166 167
    0x4006, 0x400b, 0x3002, 0x3000, 0x1001, 0x3004, 0x4008, 0x3006,
    // 170 171 172 173 174 175 176 177
    0x4009, 0x400d, 0x4003, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,

    // 200 201 202 203 204 205 206 207
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    // 210 211 212 213 214 215 216 217
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    // 220 221 222 223 224 225 226 227
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    // 230 231 232 233 234 235 236 237
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    // 240 241 242 243 244 245 246 247
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    // 250 251 252 253 254 255 256 257
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    // 260 261 262 263 264 265 266 267
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    // 270 271 272 273 274 275 276 277
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,

    // 300 301 302 303 304 305 306 307
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    // 310 311 312 313 314 315 316 317
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    // 320 321 322 323 324 325 326 327
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    // 330 331 332 333 334 335 336 337
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    // 340 341 342 343 344 345 346 347
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    // 350 351 352 353 354 355 356 357
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    // 360 361 362 363 364 365 366 367
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    // 370 371 372 373 374 375 376 377
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


void setup () {
    keyer_mode = MODE_PADDLE_NORMAL;

    pinMode(LEFT_PADDLE, INPUT);
    digitalWrite(LEFT_PADDLE, HIGH);
    pinMode(RIGHT_PADDLE, INPUT);
    digitalWrite(RIGHT_PADDLE, HIGH);

    pinMode(SIDETONE, OUTPUT);
    digitalWrite(SIDETONE, LOW);

    last_ms = 100 + millis();
    next_state_transition_ms = last_ms;
    keyer_state = KEY_UP;
    next_keyer_state = KEY_UP;

    display_manager.init();
    
    display_manager.key_mode(key_modestrings[0]);
    display_manager.sidetone(SIDETONE_FREQUENCY);
    display_manager.input_source(input_strings[keyer_mode]);
    display_manager.xmit_mode(xmit_strings[0]);

    // Without the serial support, the firmware takes 11316 bytes
    // Initial support, 12054 bytes
    // With space 12096
    // with working space 12164
    Serial.begin(115200);
    while (!Serial) {
    }
}

void loop() {
    unsigned long now = millis();
    unsigned wpm;

    if (now >= next_state_transition_ms) {
	if (MODE_PADDLE_NORMAL == keyer_mode) {
	    display_manager.input_source(input_strings[keyer_mode]);
	    switch(keyer_state) {
	    case KEY_DIT:
		transmitter.key_up();
		next_state_transition_ms = now + dot_twitches;
		keyer_state = KEY_UP;
		if (!digitalRead(RIGHT_PADDLE)) {
		    next_keyer_state = KEY_DAH;
		}
		break;

	    case KEY_DAH:
		transmitter.key_up();
		next_state_transition_ms = now + dot_twitches;
		keyer_state = KEY_UP;
		if (!digitalRead(LEFT_PADDLE)) {
		    next_keyer_state = KEY_DIT;
		}
		break;

	    case KEY_UP:
		// I ought to be able to build this into a table and make it table driven
		switch(next_keyer_state) {
		case KEY_DIT:
		    next_state_transition_ms = now + dot_twitches;
		    transmitter.key_down();
		    break;

		case KEY_DAH:
		    next_state_transition_ms = now + dash_twitches;
		    transmitter.key_down();
		    break;

		default:
		    if (!digitalRead(LEFT_PADDLE)) {
			next_keyer_state = KEY_DIT;
			next_state_transition_ms = now + dot_twitches;
			transmitter.key_down();
		    }
		    else if (!digitalRead(RIGHT_PADDLE)) {
			next_keyer_state = KEY_DAH;
			next_state_transition_ms = now + dash_twitches;
			transmitter.key_down();
		    }
		    break;
		}
		keyer_state = next_keyer_state;
		next_keyer_state = KEY_UP;
		break;
	    }
	}
	else {
	    if (KEY_UP == keyer_state) {
		transmitter.key_down();
		if (1 & kbd_bit) {
		    keyer_state = KEY_DAH;
		    next_state_transition_ms = now + dash_twitches;
		}
		else {
		    keyer_state = KEY_DIT;
		    next_state_transition_ms = now + dot_twitches;
		}
	    }
	    else {
		transmitter.key_up();
		keyer_state = KEY_UP;
		--kbd_count;
		if (kbd_count > 0) {
		    kbd_bit >>= 1;
		    next_state_transition_ms = now + dot_twitches;
		}
		else {
		    next_state_transition_ms = now + dash_twitches;
		    keyer_mode = MODE_PADDLE_NORMAL;
		}
	    }
	    
	}
    }
    if (MODE_SERIAL != keyer_mode) {
	if (Serial.available()) {
	    char c;
	    uint16_t z;

	    Serial.readBytes(&c, 1);
	    z = morse_table[c];
	    if (z) {
		if (0xd000 == z) {
		    keyer_mode = MODE_SERIAL;
		    keyer_state = KEY_DAH;
		    kbd_count = 1;
		    next_state_transition_ms = now + dash_twitches + dot_twitches;
		    display_manager.scrolling_text(' ');
		}
		else if (0xd000 > z) {
		    keyer_mode = MODE_SERIAL;
		    display_manager.input_source(input_strings[keyer_mode]);
		    kbd_count = z >> 12;
		    kbd_bit = z & 0x0fff;
		    display_manager.scrolling_text(c);
		}
		else {
		    keyer_mode = MODE_PADDLE_NORMAL;
		}
	    }
	    else {
		keyer_mode = MODE_PADDLE_NORMAL;
	    }
	}
    }
    wpm = find_wpm();
    dot_twitches = MS_PER_DOT/wpm;
    dash_twitches = MS_PER_DASH/wpm;
    word_twitches = MS_PER_WORD/wpm;
}	
