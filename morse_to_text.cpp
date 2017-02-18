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

void morse_to_text::update(mtt_symbol s) {
    switch (s) {
    case Dit: case Dah:
	m_state = morse_decode_table[m_state].links[s];
	break;

    case CharSpace:
	if (0 != morse_decode_table[m_state].c) {
	    if (CONFIG_MANAGER_GET_PROGRAM_MODE()) {
		CONFIG_MANAGER_PROCESS_COMMAND(morse_decode_table[m_state].c);
	    }
	    else {
		DISPLAY_MANAGER_SCROLLING_TEXT(morse_decode_table[m_state].c);
	    }
	}
	m_state = 0;
	break;

    case WordSpace:
	DISPLAY_MANAGER_SCROLLING_TEXT(' ');
	break;

    default:
	m_state = 0;
    }
}

#endif // FEATURE_MORSE_TO_TEXT
