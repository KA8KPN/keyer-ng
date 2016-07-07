#include <Arduino.h>

#include "keying.h"
#include "keyer.h"
#include "display.h"

keying::keying(void) {}
keying::~keying(void) {}

class transmitter : public keying {
public:
    transmitter(int ptt_line, int key_out_line, int sidetone_freq);
    virtual void key_up(void);
    virtual void key_down(void);
    virtual void ptt_push(void);
    virtual void ptt_release(void);
    virtual void set_sidetone_freq(int sidetone_freq);
    virtual void toggle_sidetone_enable(void);
    virtual ~transmitter(void);

protected:
    void set_ptt(int ptt_line);
    void set_key_out(int key_out_line);
    byte m_pttLine;
    byte m_keyOutLine;
    int m_sidetoneFreq;
    void (*m_keyAction)(byte, byte);
    void (*m_pttAction)(byte, byte);
};


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

transmitter::transmitter(int ptt_line, int key_out_line, int sidetone_freq) {
    m_keyAction = null_action;
    m_pttAction = null_action;
    take_sidetone_action = null_sidetone;

    set_ptt(ptt_line);
    set_key_out(key_out_line);
    set_sidetone_freq(sidetone_freq);
    DISPLAY_MANAGER_SIDETONE(m_sidetoneFreq);
}

void transmitter::set_ptt(int ptt_line) {
    m_pttAction(m_pttLine, LOW);
    m_pttLine = ptt_line;
    if (0 != ptt_line) {
	m_pttAction = digitalWrite;
    }
    else {
	m_pttAction = null_action;
    }
}

void transmitter::set_key_out(int key_out_line) {
    m_keyAction(m_keyOutLine, LOW);
    m_keyOutLine = key_out_line;
    if (0 != key_out_line) {
	m_keyAction = digitalWrite;
    }
    else {
	m_keyAction = null_action;
    }
}

void transmitter::set_sidetone_freq(int sidetone_freq) {
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


void transmitter::key_up(void) {
    m_keyAction(m_keyOutLine, LOW);
    take_sidetone_action(m_sidetoneFreq, false);
}

void transmitter::key_down(void) {
    m_keyAction(m_keyOutLine, HIGH);
    take_sidetone_action(m_sidetoneFreq, true);
}

void transmitter::ptt_push(void) {
    m_pttAction(m_pttLine, HIGH);
}

void transmitter::ptt_release(void) {
    m_pttAction(m_pttLine, HIGH);
}


void transmitter::toggle_sidetone_enable(void) {
    if (null_sidetone == take_sidetone_action) {
	set_sidetone_freq(m_sidetoneFreq);
    }
    else {
	set_sidetone_freq(0);
    }
}


transmitter::~transmitter(void) {
}

keying *system_transmitter = NULL;
static keying *actual_transmitter = NULL;


class cpo : public keying {
public:
    cpo(int sidetone_freq);
    virtual void set_sidetone_freq(int sidetone_freq);
    virtual void toggle_sidetone_enable(void);
    virtual void key_up(void);
    virtual void key_down(void);
    virtual void ptt_push(void) {}
    virtual void ptt_release(void) {}
    virtual ~cpo(void);

private:
    int m_sidetoneFreq;
};

cpo::cpo(int sidetone_freq) : keying() {
    m_sidetoneFreq = sidetone_freq;
}

void cpo::set_sidetone_freq(int sidetone_freq) {
    actual_transmitter->set_sidetone_freq(sidetone_freq);
    if (0 != sidetone_freq) {
	m_sidetoneFreq = sidetone_freq;
    }
}

void cpo::toggle_sidetone_enable(void) {
    actual_transmitter->toggle_sidetone_enable();
}

void cpo::key_up(void) {
    noTone(SIDETONE);
}

void cpo::key_down(void) {
    tone(SIDETONE, m_sidetoneFreq);
}

cpo::~cpo() {}

static keying *code_practice_oscillator = NULL;

void keying_initialize(void) {
    pinMode(SIDETONE, OUTPUT);
    digitalWrite(SIDETONE, LOW);

    actual_transmitter = new transmitter(PTT_1, KEY_OUT_1, SIDETONE_FREQUENCY);
    code_practice_oscillator = new cpo(SIDETONE_FREQUENCY);
    system_transmitter = actual_transmitter;
    DISPLAY_MANAGER_XMIT_MODE(1);
}

void keying_config_mode(boolean enter_config_mode) {
    if (enter_config_mode) {
	system_transmitter = code_practice_oscillator;
    }
    else {
	system_transmitter = actual_transmitter;
    }
}
