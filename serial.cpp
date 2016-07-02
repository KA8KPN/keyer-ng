#include "options.h"

#ifdef FEATURE_SERIAL_INPUT

#include "Arduino.h"

#include "serial.h"
#include "morse_to_text.h"

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
    if (MORSE_TO_TEXT_BUFFER_NOT_FULL()) {
	if (Serial.available()) {
	    char c;

	    Serial.readBytes(&c, 1);
	    if (m_echoChars) {
		Serial.write(c);
		if ('\r' == c) {
		    Serial.write('\n');
		}
	    }
	    MORSE_TO_TEXT_ADD_TO_BUFFER(c);
	}
    }
}

#endif // FEATURE_SERIAL_INPUT
