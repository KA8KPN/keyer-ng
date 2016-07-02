#include "Arduino.h"

#include "serial.h"

void serial_setup(void) {
    delay(1000);
    Serial.begin(115200);
    while (!Serial) {
    }
}


serial::serial(morse_to_text *mtt, bool echo) : m_mtt(mtt), m_echoChars(echo) {
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
