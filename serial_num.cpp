#include "options.h"

#ifdef FEATURE_SERIAL_NUMBER

#include "Arduino.h"

#include "serial_num.h"
#include "morse_tables.h"
#include "morse_to_text.h"

serial_num system_serial_num;

void serial_num_initialize(void) {
    DISPLAY_MANAGER_NUMBER(1);
}


serial_num::serial_num(void) {
    m_inMacro = false;
    m_count = 1;
    m_increment = 0;
}

bool serial_num::update(unsigned long now, uint8_t m) {
    if (!m_inMacro) {
	switch(m) {
	case 0:
	    // Send serial number with cut numbers
	    // The K3NG keyer just does "T" and "N" for 0 and 9
	    m_inMacro = true;
	    sprintf(m_buffer, "%d", m_count);
	    {
		char *s;
		s = m_buffer;
		while('\0' != *s) {
		    if ('0' == *s) {
			*s = 'T';
		    }
		    else if ('9' == *s) {
			*s = 'N';
		    }
		    ++s;
		}
	    }
	    m_increment = 1;
	    m_newChar = true;
	    break;

	case 1:
	    // Send serial number as digits
	    m_inMacro = true;
	    sprintf(m_buffer, "%d", m_count);
	    m_increment = 1;
	    m_newChar = true;
	    break;

	case 2:
	    // Decrement count
	    m_increment = -1;
	    break;
	}

	m_bufPtr = 0;
	m_nextStateTransitionMs = 0;
	m_keyerState = KEY_UP;
	m_charBit = 0;
	m_charCount = 0;
    }
    if (now >= m_nextStateTransitionMs) {
	if (!m_newChar) {
	    unsigned ptt_delay;
	    if (KEY_UP == m_keyerState) {
		ptt_delay = TRANSMITTER_KEY_DOWN();
		if (1 & m_charBit) {
		    m_keyerState = KEY_DAH;
		    m_nextStateTransitionMs = now + WPM_DASH_TWITCHES() + ptt_delay;
		    MORSE_TO_TEXT_UPDATE(Dah);
		}
		else {
		    m_keyerState = KEY_DIT;
		    m_nextStateTransitionMs = now + WPM_DOT_TWITCHES() + ptt_delay;
		    MORSE_TO_TEXT_UPDATE(Dit);
		}
	    }
	    else {
		TRANSMITTER_KEY_UP();
		m_keyerState = KEY_UP;
		--m_charCount;
		if (m_charCount > 0) {
		    m_charBit >>= 1;
		    m_nextStateTransitionMs = now + WPM_DOT_TWITCHES();
		}
		else {
		    m_nextStateTransitionMs = now + WPM_DASH_TWITCHES();
		    MORSE_TO_TEXT_UPDATE(CharSpace);
		    m_newChar = true;
		}
	    }
	}
    }
    if (m_newChar) {
	m_newChar = false;
	if ('\0' != m_buffer[m_bufPtr]) {
	    uint16_t z = morse_table[(size_t) m_buffer[m_bufPtr]];
	    // z = 0x501f
	    m_charCount = z >> 12;
	    m_charBit = z & 0x0fff;
	    ++m_bufPtr;
	}
	else {
	    m_inMacro = false;
	    MORSE_TO_TEXT_UPDATE(CharSpace);
	}
    }
    return !m_inMacro;
}

void serial_num::increment(void) {
    m_count += m_increment;
    m_increment = 0;
    DISPLAY_MANAGER_NUMBER(m_count);
}

#endif // FEATURE_SERIAL_INPUT
