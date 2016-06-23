#include <Arduino.h>

#include "paddles.h"
#include "morse_tables.h"

paddles::paddles(keying *transmitter, display *display_manager, const wpm *wpm) : m_transmitter(transmitter), m_displayManager(display_manager), m_wpm(wpm) {
    m_nextStateTransitionMs = 100 + millis();
    m_keyerState = KEY_UP;
    m_nextKeyerState = KEY_UP;
    m_morseTableState = 0;
}


keyer_mode_t paddles::update(unsigned long now, keyer_mode_t keyer_mode) {
    if (now >= m_nextStateTransitionMs) {
	if (MODE_PADDLE_NORMAL == keyer_mode) {
	    m_displayManager->input_source(input_strings[keyer_mode]);
	    switch(m_keyerState) {
	    case KEY_DIT:
		m_transmitter->key_up();
		m_nextStateTransitionMs = now + m_wpm->dot_twitches();
		m_keyerState = KEY_UP;
		if (!digitalRead(RIGHT_PADDLE)) {
		    m_nextKeyerState = KEY_DAH;
		}
		break;

	    case KEY_DAH:
		m_transmitter->key_up();
		m_nextStateTransitionMs = now + m_wpm->dot_twitches();
		m_keyerState = KEY_UP;
		if (!digitalRead(LEFT_PADDLE)) {
		    m_nextKeyerState = KEY_DIT;
		}
		break;

	    case KEY_UP:
		// I ought to be able to build this into a table and make it table driven
		switch(m_nextKeyerState) {
		case KEY_DIT:
		    m_nextStateTransitionMs = now + m_wpm->dot_twitches();
		    m_transmitter->key_down();
		    break;

		case KEY_DAH:
		    m_nextStateTransitionMs = now + m_wpm->dash_twitches();
		    m_transmitter->key_down();
		    break;

		default:
		    if (!digitalRead(LEFT_PADDLE)) {
			m_nextKeyerState = KEY_DIT;
			m_nextStateTransitionMs = now + m_wpm->dot_twitches();
			m_transmitter->key_down();
		    }
		    else if (!digitalRead(RIGHT_PADDLE)) {
			m_nextKeyerState = KEY_DAH;
			m_nextStateTransitionMs = now + m_wpm->dash_twitches();
			m_transmitter->key_down();
		    }
		    break;
		}
		m_keyerState = m_nextKeyerState;
		m_nextKeyerState = KEY_UP;
		break;
	    }
	}
    }
    return keyer_mode;
}
