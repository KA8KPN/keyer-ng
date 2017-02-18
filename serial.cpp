#include "options.h"

#ifdef FEATURE_SERIAL_INPUT

#include "Arduino.h"

#include "serial.h"
#include "text_to_morse.h"

serial *system_serial = NULL;

void serial_initialize(void) {
    delay(1000);
    Serial.begin(115200);
    while (!Serial) {
    }

    system_serial = new serial(true);
}


serial::serial(bool echo) : m_echoChars(echo) {
}

void serial::update(void) {
    if (TEXT_TO_MORSE_BUFFER_NOT_FULL()) {
	if (Serial.available()) {
	    char c;

	    Serial.readBytes(&c, 1);
	    if (m_echoChars) {
		Serial.write(c);
		if ('\r' == c) {
		    Serial.write('\n');
		}
	    }
	    TEXT_TO_MORSE_ADD_TO_BUFFER(c);
	}
    }
}

#endif // FEATURE_SERIAL_INPUT
