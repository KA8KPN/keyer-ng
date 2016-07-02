#ifndef PS2_KEYBOARD_H_INCLUDED
#define PS2_KEYBOARD_H_INCLUDED

#include "keyer.h"
#include "keying.h"
#include "display.h"
#include "wpm.h"
#include "morse_to_text.h"
#include "PS2Keyboard.h"

void ps2_keyboard_setup(void);

class ps2_keyboard {
public:
    ps2_keyboard(morse_to_text *mtt);
    void update(void);

private:
    morse_to_text *m_mtt;
};

#endif // PS2_KEYBOARD_H_INCLUDED
