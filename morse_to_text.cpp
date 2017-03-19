#include "options.h"

#ifdef FEATURE_MORSE_TO_TEXT

#include "Arduino.h"

#include "morse_to_text.h"
#include "morse_tables.h"
#include "config_manager.h"
#include "display.h"
#include "memories.h"

morse_to_text system_mtt;

void morse_to_text_initialize(void) {
}

morse_to_text::morse_to_text() {
    m_state = 0;
    m_commandBuffer[0] = '\0';
    m_buffPtr = 0;
    m_spaceSentLast = false;
    m_special = false;
}

/*
 * How commands work:
 *
 * In command mode, the morse_to_text class will accumulate characters in a buffer and will pass the
 * last few characters sent to the configuration manager for processing when the "WordSpace" update
 * is processed.   What that means is that the commands will all be treated as words.  Since the
 * longest commands are three characters, the buffer will be four characters long.
 *
 * I'll have to think on the best way to implement this, but it shouldn't be too tough.
 */

// TODO:  If it ever returns to the start state, it should indicate that there was a symbol
// TODO:  it did not decode.
void morse_to_text::update(mtt_symbol s) {
    switch (s) {
    case Dit: case Dah:
	m_state = morse_decode_table[m_state].links[s];
	break;

    case CharSpace:
	if (0 != morse_decode_table[m_state].c) {
	    uint16_t position;

	    // serial_log("In CharSpace decoded '%c'\r\n", morse_decode_table[m_state].c);
	    DISPLAY_MANAGER_SCROLLING_TEXT(morse_decode_table[m_state].c);
	    if (CONFIG_MANAGER_GET_COMMAND_MODE()) {
		if (m_buffPtr > 2) {
		    m_commandBuffer[0] = m_commandBuffer[1];
		    m_commandBuffer[1] = m_commandBuffer[2];
		    m_commandBuffer[2] = morse_decode_table[m_state].c;
		}
		else {
		    m_commandBuffer[m_buffPtr++] = morse_decode_table[m_state].c;
		}
		m_commandBuffer[m_buffPtr] = '\0';
	    }
	    if (0 != (position = MEMORY_RECORDING())) {
		// serial_log("Memory is recording\r\n");
		if (m_special) {
		    // serial_log("Special found\r\n");
		    m_special = false;
		    if (('c' == morse_decode_table[m_state].c) || ('C' == morse_decode_table[m_state].c)) {
			// serial_log("Record 0x40\r\n");
			RECORD_SPECIAL_ELEMENT(m_specialStart, 0x40);
		    }
		    else if (('e' == morse_decode_table[m_state].c) || ('E' == morse_decode_table[m_state].c)) {
			// serial_log("Record 0x41\r\n");
			RECORD_SPECIAL_ELEMENT(m_specialStart, 0x41);
		    }
		    else if (('n' == morse_decode_table[m_state].c) || ('N' == morse_decode_table[m_state].c)) {
			// serial_log("Record 0x42\r\n");
			RECORD_SPECIAL_ELEMENT(m_specialStart, 0x42);
		    }
		}
		else {
		    if ('\x01' == morse_decode_table[m_state].c) {
			// serial_log("Found a backslash\r\n");
			m_special = true;
			m_specialStart = position;
		    }
		}
	    }

	    m_spaceSentLast = false;
	}
	m_state = 0;
	break;

    case WordSpace:
	if (!m_spaceSentLast) {
	    if (CONFIG_MANAGER_GET_COMMAND_MODE()) {
		CONFIG_MANAGER_PROCESS_COMMAND(m_commandBuffer);
		m_buffPtr = 0;
		m_commandBuffer[0] = '\0';
	    }
	    m_spaceSentLast = true;
	    DISPLAY_MANAGER_SCROLLING_TEXT(' ');
	}
	m_state = 0;
	break;

    default:
	m_state = 0;
	break;
    }
}

#endif // FEATURE_MORSE_TO_TEXT
