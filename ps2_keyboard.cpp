#include "options.h"

#ifdef FEATURE_PS2_KEYBOARD

#include <Arduino.h>

#include "ps2_keyboard.h"

static PS2Keyboard keyboard;
ps2_keyboard *system_ps2_keyboard = NULL;

void ps2_keyboard_initialize(void) {
    keyboard.begin(KBD_DATA, KBD_IRQ);
    system_ps2_keyboard = new ps2_keyboard;
}


ps2_keyboard::ps2_keyboard(void) {
}

void ps2_keyboard::update(void) {
    if (MORSE_TO_TEXT_BUFFER_NOT_FULL()) {
	if (keyboard.available()) {
	    char c;

	    c = keyboard.read();
	    MORSE_TO_TEXT_ADD_TO_BUFFER(c);
	}
    }
}
#endif // !FEATURE_PS2_KEYBOARD
