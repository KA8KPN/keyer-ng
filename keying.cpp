#include <Arduino.h>

#include "keying.h"

static void take_action(int line, bool key_down) {
}

static void sidetone_action(int frequency, bool key_down) {
    if (key_down) {
	tone(SIDETONE, frequency);
    }
    else {
	noTone(SIDETONE);
    }
}

static void null_action(int line, bool key_down) {
    (void) line;
    (void) key_down;
}

keying::keying(int ptt_line, int key_out_line, int sidetone_freq) {
    m_key_action = null_action;
    m_ptt_action = null_action;
    m_sidetone_action = null_action;

    set_ptt(ptt_line);
    set_key_out(key_out_line);
    set_sidetone_freq(sidetone_freq);
}

void keying::set_ptt(int ptt_line) {
    m_ptt_action(m_ptt_line, false);
    m_ptt_line = ptt_line;
    if (0 != ptt_line) {
	m_ptt_action = take_action;
    }
    else {
	m_ptt_action = null_action;
    }
}

void keying::set_key_out(int key_out_line) {
    m_key_action(m_key_out_line, false);
    m_key_out_line = key_out_line;
    if (0 != key_out_line) {
	m_key_action = take_action;
    }
    else {
	m_key_action = null_action;
    }
}

void keying::set_sidetone_freq(int sidetone_freq) {
    m_sidetone_action(m_sidetone_freq, false);
    m_sidetone_freq = sidetone_freq;
    if (0 != sidetone_freq) {
	m_sidetone_action = sidetone_action;
    }
    else {
	m_sidetone_action = null_action;
    }
}


void keying::key_up(void) {
    m_key_action(m_key_out_line, false);
    m_sidetone_action(m_sidetone_freq, false);
}

void keying::key_down(void) {
    m_key_action(m_key_out_line, true);
    m_sidetone_action(m_sidetone_freq, true);
}

void keying::ptt_pushed(void) {
    m_ptt_action(m_ptt_line, true);
}

void keying::ptt_released(void) {
    m_ptt_action(m_ptt_line, false);
}

