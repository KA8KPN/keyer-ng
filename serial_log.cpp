#include "options.h"

#ifdef FEATURE_SERIAL_LOG

#include "Arduino.h"
#include <stdarg.h>

void serial_log_initialize(void) {
    delay(1000);
    Serial.begin(115200);
    while (!Serial) {
    }
}

int serial_log(const char *format, ...) {
    char buff[100];
    int result;
    va_list ap;

    va_start(ap, format);
    result = vsprintf(buff, format, ap);
    va_end(ap);

    Serial.write(buff, strlen(buff));
    return result;
}

#endif // FEATURE_SERIAL_LOG
