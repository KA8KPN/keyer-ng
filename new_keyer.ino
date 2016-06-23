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

#define DEFINE_MORSE_TABLES
#include "morse_tables.h"

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
