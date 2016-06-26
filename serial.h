#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#include "keyer.h"
#include "keying.h"
#include "display.h"
#include "wpm.h"

class serial {
public:
    serial(keying *transmitter, display *display_manager, const wpm *wpm, bool echo);
    keyer_mode_t update(unsigned long now, keyer_mode_t mode);

private:
    keying *m_transmitter;
    display *m_displayManager;
    const wpm *m_wpm;
    bool m_echoChars;
    unsigned long m_nextStateTransitionMs;
    keyer_state_t m_keyerState;
    uint8_t m_kbdBit, m_kbdCount;
};

#endif // SERIAL_H_INCLUDED
