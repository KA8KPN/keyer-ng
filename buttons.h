#ifndef BUTTONS_H_INCLUDED
#define BUTTONS_H_INCLUDED

#include "options.h"

#if defined(FEATURE_BUTTONS)

class buttons {
public:
    buttons(byte aiPin);
    void update(void);

private:
    byte m_aiPin;
    uint8_t m_oldButton;
};

extern buttons *system_buttons;
void buttons_initialize(void);

#define BUTTONS_INITIALIZE()    buttons_initialize()
#define BUTTONS_UPDATE()        system_buttons->update()
#else // !defined(FEATURE_SPEED_CONTROL)
#define BUTTONS_INITIALIZE()
#define BUTTONS_UPDATE()

#endif // defined(FEATURE_BUTTONS)

#endif // !BUTTONS_H_INCLUDED
