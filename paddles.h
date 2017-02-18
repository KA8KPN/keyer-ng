#ifndef PADDLES_H_INCLUDED
#define PADDLES_H_INCLUDED

#include "keyer.h"
#include "keying.h"

class paddles {
public:
    paddles(void);
    void set_paddle_ports(byte right_paddle, byte left_paddle);
    input_mode_t update(unsigned long now, input_mode_t mode);
    input_mode_t toggle_reverse(void);

private:
    keyer_state_t m_keyerState;
    keyer_state_t m_lastKeyerState;

    unsigned long m_nextStateTransitionMs;
    unsigned long m_startReadingPaddlesMs;
    uint8_t m_leftPaddle, m_rightPaddle;
    uint8_t m_ditPaddle, m_dahPaddle;
    bool m_ditClosed, m_dahClosed;
    input_mode_t m_paddleMode;
    unsigned long m_addSpaceMs;
};

extern paddles system_paddles;
void paddles_initialize(byte right_paddle, byte left_paddle);

#define PADDLES_INITIALIZE(right_paddle, left_paddle) paddles_initialize(right_paddle, left_paddle)
#define PADDLES_REVERSE() system_paddles.toggle_reverse()
#define PADDLES_UPDATE(now, keyer_mode) system_paddles.update(now, keyer_mode)
#define PADDLES_POINTER system_paddles

#endif // PADDLES_H_INCLUDED
