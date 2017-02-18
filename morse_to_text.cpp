#include "options.h"

#ifdef FEATURE_MORSE_TO_TEXT

#include "Arduino.h"

#include "morse_to_text.h"
#include "morse_tables.h"
#include "config_manager.h"
#include "display.h"

morse_to_text system_mtt;

void morse_to_text_initialize(void) {
}

morse_to_text::morse_to_text() {
    m_state = 0;
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

void morse_to_text::update(mtt_symbol s) {
    switch (s) {
    case Dit: case Dah:
	m_state = morse_decode_table[m_state].links[s];
	break;

    case CharSpace:
	if (0 != morse_decode_table[m_state].c) {
	    DISPLAY_MANAGER_SCROLLING_TEXT(morse_decode_table[m_state].c);
	}
	m_state = 0;
	break;

    case WordSpace:
	if (CONFIG_MANAGER_GET_COMMAND_MODE()) {
	    CONFIG_MANAGER_PROCESS_COMMAND(morse_decode_table[m_state].c);
	}
	else {
	    DISPLAY_MANAGER_SCROLLING_TEXT(' ');
	}
	break;

    default:
	m_state = 0;
    }
}

#endif // FEATURE_MORSE_TO_TEXT
