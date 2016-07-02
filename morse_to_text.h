#ifndef MORSE_TO_TEXT_H_INCLUDED
#define MORSE_TO_TEXT_H_INCLUDED

#include "options.h"

#ifdef FEATURE_MORSE_TO_TEXT

#include "keyer.h"
#include "keying.h"
#include "display.h"
#include "wpm.h"

class morse_to_text {
public:
    morse_to_text(void);
    input_mode_t update(unsigned long now, input_mode_t mode);
    bool buffer_not_full(void);
    void add_to_buffer(char c);

private:
    bool buffer_not_empty(void) { return m_bPtr != m_ePtr;}
    char read_from_buffer(void);
    unsigned long m_nextStateTransitionMs;
    keyer_state_t m_keyerState;
    uint8_t m_kbdBit, m_kbdCount;
    char m_buffer[128];
    uint8_t m_bPtr, m_ePtr;
};
extern morse_to_text *system_mtt;
void morse_to_text_initialize(void);
#define MORSE_TO_TEXT_INITIALIZE()      morse_to_text_initialize()
#define MORSE_TO_TEXT_UPDATE(now, mode) system_mtt->update(now, mode)
#define MORSE_TO_TEXT_BUFFER_NOT_FULL() system_mtt->buffer_not_full()
#define MORSE_TO_TEXT_ADD_TO_BUFFER(c)  system_mtt->add_to_buffer(c)
#else // !FEATURE_MORSE_TO_TEXT
#define MORSE_TO_TEXT_INITIALIZE()
#define MORSE_TO_TEXT_UPDATE(now, mode) CONFIG_MANAGER_PADDLES_MODE()
#define MORSE_TO_TEXT_BUFFER_NOT_FULL() (true)
#define MORSE_TO_TEXT_ADD_TO_BUFFER(c)  ((void)(c))
#endif // !FEATURE_MORSE_TO_TEXT

#endif // MORSE_TO_TEXT_H_INCLUDED
