#include <Arduino.h>

#include "paddles.h"
#include "display.h"
#include "morse_tables.h"
#include "wpm.h"
#include "config_manager.h"

paddles system_paddles;

void paddles_initialize(byte right_paddle, byte left_paddle)
{
    system_paddles.set_paddle_ports(right_paddle, left_paddle);
    pinMode(left_paddle, INPUT);
    digitalWrite(left_paddle, HIGH);
    pinMode(right_paddle, INPUT);
    digitalWrite(right_paddle, HIGH);
    DISPLAY_MANAGER_KEY_MODE(KEYER_IAMBIC_A);
}

paddles::paddles(void) : m_leftPaddle(0), m_rightPaddle(0), m_paddleMode(MODE_PADDLE_NORMAL) {
    m_nextStateTransitionMs = 100 + millis();
    m_startReadingPaddlesMs = 0;
    m_keyerState = KEY_UP;
    m_lastKeyerState = KEY_UP;
    m_morseTableState = 0;
    m_ditPaddle = m_leftPaddle;
    m_dahPaddle = m_rightPaddle;
    m_ditClosed = false;
    m_dahClosed = false;
    m_addSpaceMs = 0;
}


void paddles::set_paddle_ports(byte right_paddle, byte left_paddle) {
    m_leftPaddle = left_paddle;
    m_rightPaddle = right_paddle;
}

input_mode_t paddles::update(unsigned long now, input_mode_t input_mode) {
    if (now >= m_startReadingPaddlesMs) {
	m_dahClosed = m_dahClosed || (0 == digitalRead(m_dahPaddle));
	m_ditClosed = m_ditClosed || (0 == digitalRead(m_ditPaddle));
    }

    if (now >= m_nextStateTransitionMs) {
	if ((MODE_PADDLE_NORMAL == input_mode) || (MODE_PADDLE_REVERSE == input_mode)) {
	    unsigned ptt_delay;
	    keyer_state_t tempKeyerState;
 
	    tempKeyerState = m_keyerState;
	    DISPLAY_MANAGER_INPUT_SOURCE(input_mode);
	    switch(m_keyerState) {
	    case KEY_DIT:
	    case KEY_DAH:
		m_keyerState = KEY_UP;
		break;

	    case KEY_UP:
		// I ought to be able to build this into a table and make it table driven
		switch(m_lastKeyerState) {
		case KEY_DIT:
		    if (m_dahClosed) {
			m_keyerState = KEY_DAH;
		    }
		    else if (m_ditClosed) {
			m_keyerState = KEY_DIT;
		    }
		    break;

		case KEY_DAH:
		default:
		    if (m_ditClosed) {
			m_keyerState = KEY_DIT;
		    }
		    else if (m_dahClosed) {
			m_keyerState = KEY_DAH;
		    }
		    break;
		}
		break;
	    }
	    m_lastKeyerState = tempKeyerState;
	    switch(m_keyerState) {
	    case KEY_DIT:
		// TODO:  If it ever returns to the start state, it should indicate that there was a symbol
		// TODO:  it did not decode.
		m_morseTableState = morse_decode_table[m_morseTableState].links[0];
		ptt_delay = TRANSMITTER_KEY_DOWN();
		m_nextStateTransitionMs = now + WPM_DOT_TWITCHES() + ptt_delay;
		m_startReadingPaddlesMs = now + WPM_DOT_TWITCHES() + ptt_delay;
		m_ditClosed = false;
		m_dahClosed = false;
		m_addSpaceMs = 0;
		break;

	    case KEY_DAH:
		// TODO:  If it ever returns to the start state, it should indicate that there was a symbol
		// TODO:  it did not decode.
		m_morseTableState = morse_decode_table[m_morseTableState].links[1];
		ptt_delay = TRANSMITTER_KEY_DOWN();
		m_nextStateTransitionMs = now + WPM_DASH_TWITCHES() + ptt_delay;
		m_startReadingPaddlesMs = now + WPM_DASH_TWITCHES() + ptt_delay;
		m_ditClosed = false;
		m_dahClosed = false;
		m_addSpaceMs = 0;
		break;

	    default:
		if (KEY_UP == m_lastKeyerState) {
		    if (0 != morse_decode_table[m_morseTableState].c) {
			if (CONFIG_MANAGER_GET_PROGRAM_MODE()) {
			    CONFIG_MANAGER_PROCESS_COMMAND(morse_decode_table[m_morseTableState].c);
			    input_mode = m_paddleMode;
			}
			else {
			    DISPLAY_MANAGER_SCROLLING_TEXT(morse_decode_table[m_morseTableState].c);
			    m_addSpaceMs = now + WPM_DASH_TWITCHES();
			}
		    }
		    m_morseTableState = 0;
		}
		else {
		    TRANSMITTER_KEY_UP();
		    m_nextStateTransitionMs = now + WPM_DOT_TWITCHES();
		}
		break;
	    }
	}
    }

    if ((0 < m_addSpaceMs) && (now >= m_addSpaceMs)) {
	DISPLAY_MANAGER_SCROLLING_TEXT(' ');
	m_addSpaceMs = 0;
    }
    return input_mode;
}


input_mode_t paddles::toggle_reverse(void) {
    if (MODE_PADDLE_NORMAL == m_paddleMode) {
	m_dahPaddle = m_leftPaddle;
	m_ditPaddle = m_rightPaddle;
	m_paddleMode = MODE_PADDLE_REVERSE;
    }
    else {
	m_ditPaddle = m_leftPaddle;
	m_dahPaddle = m_rightPaddle;
	m_paddleMode = MODE_PADDLE_NORMAL;
    }
    return m_paddleMode;
}
