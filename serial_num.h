#ifndef SERIAL_NUM_H_INCLUDED
#define SERIAL_NUM_H_INCLUDED

#include "options.h"

#ifdef FEATURE_SERIAL_NUMBER
#include "keyer.h"
#include "keying.h"
#include "display.h"
#include "wpm.h"

void serial_num_initialize(void);

class serial_num {
public:
    serial_num(void);
    bool update(unsigned long now, uint8_t m);

private:
    char m_buffer[10];
    int m_bufPtr;
    bool m_inMacro;
    unsigned m_count;
    bool m_newChar;
    unsigned long m_nextStateTransitionMs;
    keyer_state_t m_keyerState;
    uint8_t m_charBit, m_charCount;
};

extern serial_num system_serial_num;
#define SERIAL_NUM_INITIALIZE()      serial_num_initialize()
#define SERIAL_NUM_UPDATE(n, m)      system_serial_num.update(n,m)
#else // ! FEATURE_SERIAL_NUMBER
#define SERIAL_NUM_INITIALIZE()
#define SERIAL_NUM_UPDATE(n, m)      (true)
#endif // ! FEATURE_SERIAL_NUMBER

#endif // SERIAL_NUM_H_INCLUDED
