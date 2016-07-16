#include <Arduino.h>

#include "keying.h"
#include "keyer.h"
#include "display.h"
#include "config_manager.h"

keying::keying(void) {}
keying::~keying(void) {}

class transmitter : public keying {
public:
    transmitter(int ptt_line, int key_out_line, int sidetone_freq, unsigned ptt_delay, unsigned ptt_hang);
    virtual void key_up(void);
    virtual unsigned key_down(void);
    virtual void ptt_push(void);
    virtual void ptt_release(void);
    virtual void set_sidetone_freq(int sidetone_freq);
    virtual void toggle_sidetone_enable(void);
    virtual void update(unsigned long now);
    virtual ~transmitter(void);

protected:
    typedef enum { keying_idle, keying_ptt_wait, keying_key_down, keying_key_up } keyingState_t;
    void key_down_actual(void);
    void set_ptt(int ptt_line);
    void set_key_out(int key_out_line);
    byte m_pttLine;
    byte m_keyOutLine;
    int m_sidetoneFreq;
    void (*m_keyAction)(byte, byte);
    void (*m_pttAction)(byte, byte);
    unsigned m_pttDelay;
    unsigned m_pttHang;
    keyingState_t m_state;
    unsigned long m_nextStateChange;
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

transmitter::transmitter(int ptt_line, int key_out_line, int sidetone_freq, unsigned ptt_delay, unsigned ptt_hang) {
    m_keyAction = null_action;
    m_pttAction = null_action;
    take_sidetone_action = null_sidetone;

    set_ptt(ptt_line);
    set_key_out(key_out_line);
    set_sidetone_freq(sidetone_freq);
    DISPLAY_MANAGER_SIDETONE(m_sidetoneFreq);
    m_pttDelay = ptt_delay;
    m_pttHang = ptt_hang;
    m_state = keying_idle;
    m_nextStateChange = 0;
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
    m_state = keying_key_up;
    m_keyAction(m_keyOutLine, LOW);
    take_sidetone_action(m_sidetoneFreq, false);
    m_nextStateChange = millis() + m_pttHang;
}

unsigned transmitter::key_down(void) {
    unsigned result = 0;
 
    // Key down may not actually key down.  If the PTT has already been triggered for longer than the delay time, then we key down.
    // If a PTT delay has been specified and the system is not in transmit mode, then we trigger the
    // ptt line, returning the delay and the update function will actually key the transmitter after
    // the appropriate ptt delay
    if (keying_idle == m_state) {
	m_state = keying_ptt_wait;
	ptt_push();
	m_nextStateChange = millis() + m_pttDelay;
	result = m_pttDelay;
    }
    else {
	key_down_actual();
    }
    return result;
}

void transmitter::key_down_actual(void) {
    m_state = keying_key_down;
    m_keyAction(m_keyOutLine, HIGH);
    take_sidetone_action(m_sidetoneFreq, true);
}

void transmitter::ptt_push(void) {
    m_pttAction(m_pttLine, HIGH);
}

void transmitter::ptt_release(void) {
    m_pttAction(m_pttLine, LOW);
}


void transmitter::toggle_sidetone_enable(void) {
    if (null_sidetone == take_sidetone_action) {
	set_sidetone_freq(m_sidetoneFreq);
    }
    else {
	set_sidetone_freq(0);
    }
}


void transmitter::update(unsigned long now) {
    if (m_nextStateChange < now) {
	switch(m_state) {
	case keying_ptt_wait:
	    key_down_actual();
	    break;

	case keying_key_up:
	    ptt_release();
	    m_state = keying_idle;
	    break;

	default:
	    break;
	}
    }
}


transmitter::~transmitter(void) {
}

keying *system_transmitter = NULL;
static keying *transmitters[5] = {NULL, NULL, NULL, NULL, NULL};


class cpo : public keying {
public:
    cpo(int sidetone_freq);
    virtual void set_sidetone_freq(int sidetone_freq);
    virtual void toggle_sidetone_enable(void);
    virtual void key_up(void);
    virtual unsigned key_down(void);
    virtual void ptt_push(void) {}
    virtual void ptt_release(void) {}
    virtual void update(unsigned long now) {};
    virtual ~cpo(void);

private:
    int m_sidetoneFreq;
};

cpo::cpo(int sidetone_freq) : keying() {
    m_sidetoneFreq = sidetone_freq;
}

void cpo::set_sidetone_freq(int sidetone_freq) {
    transmitters[CONFIG_MANAGER_XMITTER()]->set_sidetone_freq(sidetone_freq);
    if (0 != sidetone_freq) {
	m_sidetoneFreq = sidetone_freq;
    }
}

void cpo::toggle_sidetone_enable(void) {
    transmitters[CONFIG_MANAGER_XMITTER()]->toggle_sidetone_enable();
}

void cpo::key_up(void) {
    noTone(SIDETONE);
}

unsigned cpo::key_down(void) {
    tone(SIDETONE, m_sidetoneFreq);
    return 0;
}

cpo::~cpo() {}

void keying_initialize(void) {
    pinMode(SIDETONE, OUTPUT);
    digitalWrite(SIDETONE, LOW);

    transmitters[0] = new cpo(SIDETONE_FREQUENCY);
    transmitters[1] = new transmitter(PTT_1, KEY_OUT_1, SIDETONE_FREQUENCY, PTT_DELAY_1, PTT_HANG_1);
    transmitters[2] = new transmitter(PTT_2, KEY_OUT_2, SIDETONE_FREQUENCY, PTT_DELAY_2, PTT_HANG_2);
    transmitters[3] = new transmitter(PTT_3, KEY_OUT_3, SIDETONE_FREQUENCY, PTT_DELAY_3, PTT_HANG_3);
    transmitters[4] = new transmitter(PTT_4, KEY_OUT_4, SIDETONE_FREQUENCY, PTT_DELAY_4, PTT_HANG_4);
    system_transmitter = transmitters[1];
    DISPLAY_MANAGER_XMIT_MODE(1);
}

void keying_config_mode(boolean enter_config_mode) {
    if (enter_config_mode) {
	system_transmitter = transmitters[0];
    }
    else {
	system_transmitter = transmitters[CONFIG_MANAGER_XMITTER()];
    }
}


void keying_select_transmitter(uint8_t xmitter) {
    // need to both unkey and put current xmitter in receive mode
    if (5 > xmitter) {
	DISPLAY_MANAGER_XMIT_MODE(xmitter);
	system_transmitter = transmitters[CONFIG_MANAGER_XMITTER()];
    }
}
