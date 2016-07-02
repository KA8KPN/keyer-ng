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
    keyer_mode_t toggle_reverse(void);

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
    keyer_mode_t m_paddleMode;
};

extern paddles *system_paddles;
void paddles_initialize(keying *transmitter, display *display_manager, const wpm *wpm, byte right_paddle, byte left_paddle);

#define PADDLES_INITIALIZE(transmitter, display_manager, wpm, right_paddle, left_paddle) paddles_initialize(transmitter, display_manager, wpm, right_paddle, left_paddle)
#define PADDLES_REVERSE() system_paddles->toggle_reverse()
#define PADDLES_UPDATE(now, keyer_mode) system_paddles->update(now, keyer_mode)
#define PADDLES_POINTER system_paddles

#endif // PADDLES_H_INCLUDED
