#ifndef MORSE_TO_TEXT_H_INCLUDED
#define MORSE_TO_TEXT_H_INCLUDED

#include "keyer.h"
#include "keying.h"
#include "display.h"
#include "wpm.h"

class morse_to_text {
public:
    morse_to_text(keying *transmitter, const wpm *wpm);
    keyer_mode_t update(unsigned long now, keyer_mode_t mode);
    bool buffer_not_full(void);
    void add_to_buffer(char c);

private:
    bool buffer_not_empty(void) { return m_bPtr != m_ePtr;}
    char read_from_buffer(void);
    keying *m_transmitter;
    const wpm *m_wpm;
    unsigned long m_nextStateTransitionMs;
    keyer_state_t m_keyerState;
    uint8_t m_kbdBit, m_kbdCount;
    char m_buffer[128];
    uint8_t m_bPtr, m_ePtr;
};

#endif // MORSE_TO_TEXT_H_INCLUDED
