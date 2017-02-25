#include "options.h"

#ifdef FEATURE_SERIAL_DEBUG

#include "Arduino.h"

#include "serial_debug.h"

void serial_debug_initialize(void) {
    delay(1000);
    Serial.begin(115200);
    while (!Serial) {
    }
}


#endif // FEATURE_SERIAL_DEBUG
