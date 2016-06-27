#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#include "keyer.h"
#include "keying.h"
#include "display.h"
#include "wpm.h"
#include "morse_to_text.h"

void serial_setup(void);

class serial {
public:
    serial(display *display_manager, morse_to_text *mtt, bool echo);
    void update(void);

private:
    display *m_displayManager;
    morse_to_text *m_mtt;
    bool m_echoChars;
};

#endif // SERIAL_H_INCLUDED
