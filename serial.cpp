#include "Arduino.h"

#include "serial.h"

serial::serial(display *display_manager, morse_to_text *mtt, bool echo) : m_displayManager(display_manager), m_mtt(mtt), m_echoChars(echo) {
}

void serial::update(void) {
    if (m_mtt->buffer_not_full()) {
	if (Serial.available()) {
	    char c;

	    Serial.readBytes(&c, 1);
	    if (m_echoChars) {
		Serial.write(c);
		if ('\r' == c) {
		    Serial.write('\n');
		}
	    }
	    m_mtt->add_to_buffer(c);
	}
    }
}
