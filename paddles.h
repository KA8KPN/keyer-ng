#ifndef PADDLES_H_INCLUDED
#define PADDLES_H_INCLUDED

#include "keyer.h"
#include "keying.h"
#include "display.h"
#include "wpm.h"

class paddles {
public:
    paddles(keying *transmitter, display *display_manager, const wpm *wpm, byte right_paddle, byte left_paddle);
    keyer_mode_t update(unsigned long now, keyer_mode_t mode);

private:
    keyer_state_t m_keyerState;
    keyer_state_t m_lastKeyerState;

    unsigned long m_nextStateTransitionMs;
    unsigned long m_startReadingPaddlesMs;
    keying *m_transmitter;
    display *m_displayManager;
    const wpm *m_wpm;
    uint8_t m_morseTableState;
    const uint8_t m_leftPaddle, m_rightPaddle;
    uint8_t m_ditPaddle, m_dahPaddle;
    bool m_ditClosed, m_dahClosed;
};

#endif // PADDLES_H_INCLUDED
