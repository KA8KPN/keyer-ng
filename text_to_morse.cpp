#include "options.h"

#ifdef FEATURE_TEXT_TO_MORSE

#include "Arduino.h"

#include "text_to_morse.h"
#include "morse_to_text.h"
#include "morse_tables.h"
#include "config_manager.h"
#include "display.h"


static const char *command_table[] = {
    "N",
    "O"
};

text_to_morse system_ttm;

void text_to_morse_initialize(void) {
}

text_to_morse::text_to_morse() {
    m_keyerState = KEY_UP;
    m_kbdBit = 0;
    m_kbdCount = 0;
    m_nextStateTransitionMs = 100 + millis();
    m_bPtr = 0;
    m_ePtr = 0;
    m_inWordSpace = false;
}

input_mode_t text_to_morse::update(unsigned long now, input_mode_t mode) {
    if (now >= m_nextStateTransitionMs) {
	if (MODE_KEYBOARD == mode) {
	    if (!m_inWordSpace) {
		unsigned ptt_delay;
		if (KEY_UP == m_keyerState) {
		    ptt_delay = TRANSMITTER_KEY_DOWN();
		    if (1 & m_kbdBit) {
			m_keyerState = KEY_DAH;
			m_nextStateTransitionMs = now + WPM_DASH_TWITCHES() + ptt_delay;
		    }
		    else {
			m_keyerState = KEY_DIT;
			m_nextStateTransitionMs = now + WPM_DOT_TWITCHES() + ptt_delay;
		    }
		}
		else {
		    TRANSMITTER_KEY_UP();
		    m_keyerState = KEY_UP;
		    --m_kbdCount;
		    if (m_kbdCount > 0) {
			m_kbdBit >>= 1;
			m_nextStateTransitionMs = now + WPM_DOT_TWITCHES();
		    }
		    else {
			m_nextStateTransitionMs = now + WPM_DASH_TWITCHES();
			mode = CONFIG_MANAGER_PADDLES_MODE();
		    }
		}
	    }
	    else {
		m_inWordSpace = false;
		mode = CONFIG_MANAGER_PADDLES_MODE();
	    }
	}
    }
    if ((MODE_KEYBOARD != mode) && buffer_not_empty()) {
	uint8_t c = read_from_buffer();
	uint16_t z = morse_table[c];
	
	if (z) {
	    if (0xd000 == z) {
		mode = MODE_KEYBOARD;
		DISPLAY_MANAGER_INPUT_SOURCE(mode, 0);
		m_keyerState = KEY_UP;
		m_kbdCount = 1;
		m_nextStateTransitionMs = now + WPM_DASH_TWITCHES() + WPM_DOT_TWITCHES();
		m_inWordSpace = true;
		DISPLAY_MANAGER_SCROLLING_TEXT(' ');
	    }
	    else if (0xd000 > z) {
		mode = MODE_KEYBOARD;
		DISPLAY_MANAGER_INPUT_SOURCE(mode, 0);
		m_kbdCount = z >> 12;
		m_kbdBit = z & 0x0fff;
		DISPLAY_MANAGER_SCROLLING_TEXT(c);
	    }
	    else if (0xe000 == (0xf000 & z)) {
		CONFIG_MANAGER_PROCESS_COMMAND(command_table[0x0fff & z]);
	    }
	}
    }
    return mode;
}


char text_to_morse::read_from_buffer(void) {
    char c;

    c = m_buffer[m_bPtr];
    if (buffer_not_empty()) {
	m_bPtr = 127 & (m_bPtr + 1);
    }
    return c;
}


bool text_to_morse::buffer_not_full(void) {
    return (((0 != m_bPtr) || (127 != m_ePtr)) && (m_ePtr != (m_bPtr - 1)));
}

void text_to_morse::add_to_buffer(char c) {
    if (buffer_not_full()) {
	m_buffer[m_ePtr] = c;
	m_ePtr = 127 & (m_ePtr + 1);
    }
}
#endif // FEATURE_TEXT_TO_MORSE
