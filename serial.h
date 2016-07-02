#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#include "options.h"

#ifdef FEATURE_SERIAL_INPUT
#include "keyer.h"
#include "keying.h"
#include "display.h"
#include "wpm.h"

void serial_initialize(void);

class serial {
public:
    serial(bool echo);
    void update(void);

private:
    bool m_echoChars;
};

extern serial *system_serial;
#define SERIAL_INITIALIZE() serial_initialize()
#define SERIAL_UPDATE() system_serial->update()
#else // ! FEATURE_SERIAL_INPUT
#define SERIAL_INITIALIZE()
#define SERIAL_UPDATE()
#endif // ! FEATURE_SERIAL_INPUT

#endif // SERIAL_H_INCLUDED
