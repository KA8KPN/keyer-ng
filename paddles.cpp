#include <Arduino.h>

#include "paddles.h"
#include "display.h"
#include "morse_tables.h"

paddles *system_paddles = NULL;

void paddles_initialize(keying *transmitter, const wpm *wpm, byte right_paddle, byte left_paddle)
{
    system_paddles = new paddles(transmitter, wpm, right_paddle, left_paddle);

    pinMode(left_paddle, INPUT);
    digitalWrite(left_paddle, HIGH);
    pinMode(right_paddle, INPUT);
    digitalWrite(right_paddle, HIGH);
}

paddles::paddles(keying *transmitter, const wpm *wpm, uint8_t right_paddle, uint8_t left_paddle) : m_transmitter(transmitter), m_wpm(wpm), m_leftPaddle(left_paddle), m_rightPaddle(right_paddle), m_paddleMode(MODE_PADDLE_NORMAL) {
    m_nextStateTransitionMs = 100 + millis();
    m_startReadingPaddlesMs = 0;
    m_keyerState = KEY_UP;
    m_lastKeyerState = KEY_UP;
    m_morseTableState = 0;
    m_ditPaddle = m_leftPaddle;
    m_dahPaddle = m_rightPaddle;
    m_ditClosed = false;
    m_dahClosed = false;
}


keyer_mode_t paddles::update(unsigned long now, keyer_mode_t keyer_mode) {
    if (now >= m_startReadingPaddlesMs) {
	m_dahClosed = m_dahClosed || (0 == digitalRead(m_dahPaddle));
	m_ditClosed = m_ditClosed || (0 == digitalRead(m_ditPaddle));
    }

    if (now >= m_nextStateTransitionMs) {
	if ((MODE_PADDLE_NORMAL == keyer_mode) || (MODE_PADDLE_REVERSE == keyer_mode)) {
	    keyer_state_t tempKeyerState;
 
	    tempKeyerState = m_keyerState;
	    DISPLAY_MANAGER_INPUT_SOURCE(keyer_mode);
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
		    else {
			m_keyerState = KEY_UP;
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
		    else {
			m_keyerState = KEY_UP;
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
		m_nextStateTransitionMs = now + m_wpm->dot_twitches();
		m_transmitter->key_down();
		m_startReadingPaddlesMs = now + m_wpm->dot_twitches();
		m_ditClosed = false;
		m_dahClosed = false;
		break;

	    case KEY_DAH:
		// TODO:  If it ever returns to the start state, it should indicate that there was a symbol
		// TODO:  it did not decode.
		m_morseTableState = morse_decode_table[m_morseTableState].links[1];
		m_nextStateTransitionMs = now + m_wpm->dash_twitches();
		m_transmitter->key_down();
		m_startReadingPaddlesMs = now + m_wpm->dash_twitches();
		m_ditClosed = false;
		m_dahClosed = false;
		break;

	    default:
		if (KEY_UP == m_lastKeyerState) {
		    if (0 != morse_decode_table[m_morseTableState].c) {
			DISPLAY_MANAGER_SCROLLING_TEXT(morse_decode_table[m_morseTableState].c);
		    }
		    m_morseTableState = 0;
		}
		else {
		    m_transmitter->key_up();
		    m_nextStateTransitionMs = now + m_wpm->dot_twitches();
		}
		break;
	    }
	}
    }
    return keyer_mode;
}


keyer_mode_t paddles::toggle_reverse(void) {
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
