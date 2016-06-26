#include <Arduino.h>

#include "options.h"

#include "keying.h"
#include "display.h"
#include "wpm.h"
#include "paddles.h"

#include "keyer.h"

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

keying transmitter(PTT_1, KEY_OUT_1, SIDETONE_FREQUENCY);
display display_manager;
wpm wpm(A0, &display_manager);
paddles paddles(&transmitter, &display_manager, &wpm, RIGHT_PADDLE, LEFT_PADDLE);

keyer_mode_t keyer_mode;
uint8_t kbd_bit, kbd_count;
bool echo_serial;

#define DEFINE_MORSE_TABLES
#include "morse_tables.h"

keyer_state_t kbd_keyer_state;

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
    kbd_keyer_state = KEY_UP;
    echo_serial = true;
}

void loop() {
    unsigned long now = millis();

    keyer_mode = paddles.update(now, keyer_mode);
    if (now >= next_state_transition_ms) {
	if (MODE_SERIAL == keyer_mode) {
	    if (KEY_UP == kbd_keyer_state) {
		transmitter.key_down();
		if (1 & kbd_bit) {
		    kbd_keyer_state = KEY_DAH;
		    next_state_transition_ms = now + wpm.dash_twitches();
		}
		else {
		    kbd_keyer_state = KEY_DIT;
		    next_state_transition_ms = now + wpm.dot_twitches();
		}
	    }
	    else {
		transmitter.key_up();
		kbd_keyer_state = KEY_UP;
		--kbd_count;
		if (kbd_count > 0) {
		    kbd_bit >>= 1;
		    next_state_transition_ms = now + wpm.dot_twitches();
		}
		else {
		    next_state_transition_ms = now + wpm.dash_twitches();
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
	    if (echo_serial) {
		Serial.write(c);
	    }
	    z = morse_table[c];
	    if (z) {
		if (0xd000 == z) {
		    keyer_mode = MODE_SERIAL;
		    display_manager.input_source(input_strings[keyer_mode]);
		    kbd_keyer_state = KEY_DAH;
		    kbd_count = 1;
		    next_state_transition_ms = now + wpm.dash_twitches() + wpm.dot_twitches();
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
    wpm.update();
}	
