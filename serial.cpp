#include "Arduino.h"

#include "serial.h"
#include "morse_tables.h"

serial::serial(keying *transmitter, display *display_manager, const wpm *wpm, bool echo) : m_transmitter(transmitter), m_displayManager(display_manager), m_wpm(wpm), m_echoChars(echo) {
    m_keyerState = KEY_UP;
    m_kbdBit = 0;
    m_kbdCount = 0;
    m_nextStateTransitionMs = 100 + millis();
}

keyer_mode_t serial::update(unsigned long now, keyer_mode_t mode) {
    if (now >= m_nextStateTransitionMs) {
	if (MODE_SERIAL == mode) {
	    if (KEY_UP == m_keyerState) {
		m_transmitter->key_down();
		if (1 & m_kbdBit) {
		    m_keyerState = KEY_DAH;
		    m_nextStateTransitionMs = now + m_wpm->dash_twitches();
		}
		else {
		    m_keyerState = KEY_DIT;
		    m_nextStateTransitionMs = now + m_wpm->dot_twitches();
		}
	    }
	    else {
		m_transmitter->key_up();
		m_keyerState = KEY_UP;
		--m_kbdCount;
		if (m_kbdCount > 0) {
		    m_kbdBit >>= 1;
		    m_nextStateTransitionMs = now + m_wpm->dot_twitches();
		}
		else {
		    m_nextStateTransitionMs = now + m_wpm->dash_twitches();
		    mode = MODE_PADDLE_NORMAL;
		}
	    }
	}
    }
    if (MODE_SERIAL != mode) {
	if (Serial.available()) {
	    char c;
	    uint16_t z;

	    Serial.readBytes(&c, 1);
	    if (m_echoChars) {
		Serial.write(c);
		if ('\r' == c) {
		    Serial.write('\n');
		}
	    }
	    z = morse_table[c];
	    if (z) {
		if (0xd000 == z) {
		    mode = MODE_SERIAL;
		    m_displayManager->input_source(input_strings[mode]);
		    m_keyerState = KEY_DAH;
		    m_kbdCount = 1;
		    m_nextStateTransitionMs = now + m_wpm->dash_twitches() + m_wpm->dot_twitches();
		    m_displayManager->scrolling_text(' ');
		}
		else if (0xd000 > z) {
		    mode = MODE_SERIAL;
		    m_displayManager->input_source(input_strings[mode]);
		    m_kbdCount = z >> 12;
		    m_kbdBit = z & 0x0fff;
		    m_displayManager->scrolling_text(c);
		}
		else {
		    mode = MODE_PADDLE_NORMAL;
		}
	    }
	    else {
		mode = MODE_PADDLE_NORMAL;
	    }
	}
    }
    return mode;
}
