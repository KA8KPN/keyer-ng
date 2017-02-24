#include <Arduino.h>

#include "paddles.h"
#include "display.h"
#include "morse_tables.h"
#include "wpm.h"
#include "config_manager.h"
#include "morse_to_text.h"

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
    m_ditClosed = false;
    m_dahClosed = false;
    m_addSpaceMs = 0;
}


void paddles::set_paddle_ports(byte right_paddle, byte left_paddle) {
    m_leftPaddle = left_paddle;
    m_rightPaddle = right_paddle;
    m_ditPaddle = m_leftPaddle;
    m_dahPaddle = m_rightPaddle;
}

input_mode_t paddles::update(unsigned long now, input_mode_t input_mode) {
    if (now >= m_startReadingPaddlesMs) {
	m_dahClosed = m_dahClosed || (0 == digitalRead(m_dahPaddle));
	m_ditClosed = m_ditClosed || (0 == digitalRead(m_ditPaddle));
    }

    if ((MODE_PADDLE_NORMAL == input_mode) || (MODE_PADDLE_REVERSE == input_mode)) {
	if (now >= m_nextStateTransitionMs) {
	    unsigned ptt_delay;
	    keyer_state_t tempKeyerState;
 
	    tempKeyerState = m_keyerState;
	    DISPLAY_MANAGER_INPUT_SOURCE(input_mode, 0);
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
		MORSE_TO_TEXT_UPDATE(Dit);
		ptt_delay = TRANSMITTER_KEY_DOWN();
		m_nextStateTransitionMs = now + WPM_DOT_TWITCHES() + ptt_delay;
		m_startReadingPaddlesMs = now + WPM_DOT_TWITCHES() + ptt_delay;
		m_ditClosed = false;
		m_dahClosed = false;
		m_addSpaceMs = 0;
		break;

	    case KEY_DAH:
		MORSE_TO_TEXT_UPDATE(Dah);
		ptt_delay = TRANSMITTER_KEY_DOWN();
		m_nextStateTransitionMs = now + WPM_DASH_TWITCHES() + ptt_delay;
		m_startReadingPaddlesMs = now + WPM_DASH_TWITCHES() + ptt_delay;
		m_ditClosed = false;
		m_dahClosed = false;
		m_addSpaceMs = 0;
		break;

	    default:
		if (KEY_UP == m_lastKeyerState) {
		    MORSE_TO_TEXT_UPDATE(CharSpace);
		    input_mode = m_paddleMode;
		    if (0 == m_addSpaceMs) {
			m_addSpaceMs = now + WPM_DASH_TWITCHES();
		    }
		}
		else {
		    TRANSMITTER_KEY_UP();
		    m_nextStateTransitionMs = now + WPM_DOT_TWITCHES();
		}
		break;
	    }
	}

	if ((0 < m_addSpaceMs) && (now >= m_addSpaceMs)) {
	    m_addSpaceMs = 0;
	    MORSE_TO_TEXT_UPDATE(WordSpace);
	}
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
