#include <Arduino.h>

#include "keying.h"
#include "keyer.h"

static void (*take_sidetone_action)(int frequency, bool key_down);

static void sidetone_action(int frequency, bool key_down) {
    if (key_down) {
	tone(SIDETONE, frequency);
    }
    else {
	noTone(SIDETONE);
    }
}

static void null_sidetone(int frequency, bool key_down) {
    (void) frequency;
    (void) key_down;
}

static void null_action(byte line, byte new_state) {
    (void) line;
    (void) new_state;
}

keying::keying(int ptt_line, int key_out_line, int sidetone_freq) {
    m_key_action = null_action;
    m_ptt_action = null_action;
    take_sidetone_action = null_sidetone;

    set_ptt(ptt_line);
    set_key_out(key_out_line);
    set_sidetone_freq(sidetone_freq);
}

void keying::set_ptt(int ptt_line) {
    m_ptt_action(m_ptt_line, LOW);
    m_ptt_line = ptt_line;
    if (0 != ptt_line) {
	m_ptt_action = digitalWrite;
    }
    else {
	m_ptt_action = null_action;
    }
}

void keying::set_key_out(int key_out_line) {
    m_key_action(m_key_out_line, LOW);
    m_key_out_line = key_out_line;
    if (0 != key_out_line) {
	m_key_action = digitalWrite;
    }
    else {
	m_key_action = null_action;
    }
}

void keying::set_sidetone_freq(int sidetone_freq) {
    take_sidetone_action(m_sidetone_freq, false);
    m_sidetone_freq = sidetone_freq;
    if (0 != sidetone_freq) {
	take_sidetone_action = sidetone_action;
    }
    else {
	take_sidetone_action = null_sidetone;
    }
}


void keying::key_up(void) {
    m_key_action(m_key_out_line, LOW);
    take_sidetone_action(m_sidetone_freq, false);
}

void keying::key_down(void) {
    m_key_action(m_key_out_line, HIGH);
    take_sidetone_action(m_sidetone_freq, true);
}

void keying::ptt_pushed(void) {
    m_ptt_action(m_ptt_line, HIGH);
}

void keying::ptt_released(void) {
    m_ptt_action(m_ptt_line, HIGH);
}

