#ifndef PS2_KEYBOARD_H_INCLUDED
#define PS2_KEYBOARD_H_INCLUDED

#include "options.h"

#ifdef FEATURE_PS2_KEYBOARD

#include "keyer.h"
#include "keying.h"
#include "display.h"
#include "wpm.h"
#include "text_to_morse.h"
#include "PS2Keyboard.h"

class ps2_keyboard {
public:
    ps2_keyboard(void);
    void update(void);

private:
};

extern ps2_keyboard system_ps2_keyboard;
void ps2_keyboard_initialize(void);
#define PS2_KEYBOARD_INITIALIZE() ps2_keyboard_initialize()
#define PS2_KEYBOARD_UPDATE()     system_ps2_keyboard.update()
#else // !FEATURE_PS2_KEYBOARD
#define PS2_KEYBOARD_INITIALIZE()
#define PS2_KEYBOARD_UPDATE()
#endif // !FEATURE_PS2_KEYBOARD

#endif // PS2_KEYBOARD_H_INCLUDED
