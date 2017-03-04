#include "options.h"

#ifdef FEATURE_PS2_KEYBOARD

#include <Arduino.h>

#include "ps2_keyboard.h"

static PS2Keyboard keyboard;
ps2_keyboard system_ps2_keyboard;

void ps2_keyboard_initialize(void) {
    keyboard.begin(KBD_DATA, KBD_IRQ);
}

ps2_keyboard::ps2_keyboard(void) {
}

void ps2_keyboard::update(void) {
    if (TEXT_TO_MORSE_BUFFER_NOT_FULL()) {
	if (keyboard.available()) {
	    char c;

	    c = keyboard.read();
	    TEXT_TO_MORSE_ADD_TO_BUFFER(c);
	}
    }
}
#endif // !FEATURE_PS2_KEYBOARD
