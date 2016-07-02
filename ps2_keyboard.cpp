#include "Arduino.h"

#include "ps2_keyboard.h"

static PS2Keyboard keyboard;

void ps2_keyboard_setup(void) {
    keyboard.begin(KBD_DATA, KBD_IRQ);
}


ps2_keyboard::ps2_keyboard(morse_to_text *mtt) : m_mtt(mtt) {
}

void ps2_keyboard::update(void) {
    if (m_mtt->buffer_not_full()) {
	if (keyboard.available()) {
	    char c;

	    c = keyboard.read();
	    m_mtt->add_to_buffer(c);
	}
    }
}
