#ifndef WPM_H_INCLUDED
#define WPM_H_INCLUDED

#include "display.h"

class wpm {
public:
    wpm(byte potPin, display *display_manager);
    void update(void);
    unsigned dot_twitches(void) const { return m_dotTwitches; }
    unsigned dash_twitches(void) const { return m_dashTwitches; }
    unsigned word_twitches(void) const { return m_wordTwitches; }

private:
    byte m_potPin;
    unsigned m_dotTwitches;
    unsigned m_dashTwitches;
    unsigned m_wordTwitches;
    display *m_displayManager;
};

#endif // !WPM_H_INCLUDED
