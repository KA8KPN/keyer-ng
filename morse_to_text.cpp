#include "Arduino.h"

#include "morse_to_text.h"
#include "morse_tables.h"
#include "config_manager.h"

morse_to_text::morse_to_text(keying *transmitter, display *display_manager, const wpm *wpm) : m_transmitter(transmitter), m_displayManager(display_manager), m_wpm(wpm) {
    m_keyerState = KEY_UP;
    m_kbdBit = 0;
    m_kbdCount = 0;
    m_nextStateTransitionMs = 100 + millis();
    m_bPtr = 0;
    m_ePtr = 0;
}

keyer_mode_t morse_to_text::update(unsigned long now, keyer_mode_t mode) {
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
		    mode = CONFIG_MANAGER_PADDLES_MODE();
		}
	    }
	}
    }
    if ((MODE_SERIAL != mode) && buffer_not_empty()) {
	char c = read_from_buffer();
	uint16_t z = morse_table[c];
	
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
	    else if (0xe000 == (0xf000 & z)) {
		CONFIG_MANAGER_PROCESS_COMMAND(0x0fff & z);
	    }
	    else {
		mode = CONFIG_MANAGER_PADDLES_MODE();
	    }
	}
	else {
	    mode = CONFIG_MANAGER_PADDLES_MODE();
	}
    }
    return mode;
}


char morse_to_text::read_from_buffer(void) {
    char c;

    c = m_buffer[m_bPtr];
    if (buffer_not_empty()) {
	m_bPtr = 127 & (m_bPtr + 1);
    }
    return c;
}


bool morse_to_text::buffer_not_full(void) {
    return (((0 != m_bPtr) || (127 != m_ePtr)) && (m_ePtr != (m_bPtr - 1)));
}

void morse_to_text::add_to_buffer(char c) {
    if (buffer_not_full()) {
	m_buffer[m_ePtr] = c;
	m_ePtr = 127 & (m_ePtr + 1);
    }
}
