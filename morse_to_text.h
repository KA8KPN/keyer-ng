#ifndef MORSE_TO_TEXT_H_INCLUDED
#define MORSE_TO_TEXT_H_INCLUDED

#include "options.h"

typedef enum { Dit=0, Dah=1, CharSpace, WordSpace } mtt_symbol;

#ifdef FEATURE_MORSE_TO_TEXT

class morse_to_text {
public:
    morse_to_text(void);
    void update(mtt_symbol s);

private:
    uint8_t m_state;
    char m_commandBuffer[4];
    uint8_t m_buffPtr;
};

extern morse_to_text system_mtt;
void morse_to_text_initialize(void);
#define MORSE_TO_TEXT_INITIALIZE()      morse_to_text_initialize()
#define MORSE_TO_TEXT_UPDATE(s)         system_mtt.update(s)
#else // !FEATURE_MORSE_TO_TEXT
#define MORSE_TO_TEXT_INITIALIZE()
#define MORSE_TO_TEXT_UPDATE(s)         ((void)s)
#endif // !FEATURE_MORSE_TO_TEXT

#endif // MORSE_TO_TEXT_H_INCLUDED
