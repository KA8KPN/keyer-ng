#ifndef TEXT_TO_MORSE_H_INCLUDED
#define TEXT_TO_MORSE_H_INCLUDED

#include "options.h"

#ifdef FEATURE_TEXT_TO_MORSE

#include "keyer.h"
#include "keying.h"
#include "display.h"
#include "wpm.h"

class text_to_morse {
public:
    text_to_morse(void);
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
    bool m_inWordSpace;
};
extern text_to_morse system_ttm;
void text_to_morse_initialize(void);
#define TEXT_TO_MORSE_INITIALIZE()      text_to_morse_initialize()
#define TEXT_TO_MORSE_UPDATE(now, mode) system_ttm.update(now, mode)
#define TEXT_TO_MORSE_BUFFER_NOT_FULL() system_ttm.buffer_not_full()
#define TEXT_TO_MORSE_ADD_TO_BUFFER(c)  system_ttm.add_to_buffer(c)
#else // !FEATURE_TEXT_TO_MORSE
#define TEXT_TO_MORSE_INITIALIZE()
#define TEXT_TO_MORSE_UPDATE(now, mode) (mode)
#define TEXT_TO_MORSE_BUFFER_NOT_FULL() (true)
#define TEXT_TO_MORSE_ADD_TO_BUFFER(c)  ((void)(c))
#endif // !FEATURE_TEXT_TO_MORSE

#endif // TEXT_TO_MORSE_H_INCLUDED
