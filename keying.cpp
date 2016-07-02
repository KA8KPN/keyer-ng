#include <Arduino.h>

#include "keying.h"
#include "keyer.h"
#include "display.h"

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
    m_keyAction = null_action;
    m_pttAction = null_action;
    take_sidetone_action = null_sidetone;

    set_ptt(ptt_line);
    set_key_out(key_out_line);
    set_sidetone_freq(sidetone_freq);
    DISPLAY_MANAGER_SIDETONE(m_sidetoneFreq);
}

void keying::set_ptt(int ptt_line) {
    m_pttAction(m_pttLine, LOW);
    m_pttLine = ptt_line;
    if (0 != ptt_line) {
	m_pttAction = digitalWrite;
    }
    else {
	m_pttAction = null_action;
    }
}

void keying::set_key_out(int key_out_line) {
    m_keyAction(m_keyOutLine, LOW);
    m_keyOutLine = key_out_line;
    if (0 != key_out_line) {
	m_keyAction = digitalWrite;
    }
    else {
	m_keyAction = null_action;
    }
}

void keying::set_sidetone_freq(int sidetone_freq) {
    take_sidetone_action(m_sidetoneFreq, false);
    if (0 != sidetone_freq) {
	m_sidetoneFreq = sidetone_freq;
	take_sidetone_action = sidetone_action;
	DISPLAY_MANAGER_SIDETONE(m_sidetoneFreq);
    }
    else {
	take_sidetone_action = null_sidetone;
	DISPLAY_MANAGER_SIDETONE(0);
    }
}


void keying::key_up(void) {
    m_keyAction(m_keyOutLine, LOW);
    take_sidetone_action(m_sidetoneFreq, false);
}

void keying::key_down(void) {
    m_keyAction(m_keyOutLine, HIGH);
    take_sidetone_action(m_sidetoneFreq, true);
}

void keying::ptt_push(void) {
    m_pttAction(m_pttLine, HIGH);
}

void keying::ptt_release(void) {
    m_pttAction(m_pttLine, HIGH);
}


void keying::toggle_sidetone_enable(void) {
    if (null_sidetone == take_sidetone_action) {
	set_sidetone_freq(m_sidetoneFreq);
    }
    else {
	set_sidetone_freq(0);
    }
}


keying *system_transmitter = NULL;

void keying_initialize(void) {
    pinMode(SIDETONE, OUTPUT);
    digitalWrite(SIDETONE, LOW);

    system_transmitter = new keying(PTT_1, KEY_OUT_1, SIDETONE_FREQUENCY);
    DISPLAY_MANAGER_XMIT_MODE(1);
}
