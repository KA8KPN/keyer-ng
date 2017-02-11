#ifndef WPM_H_INCLUDED
#define WPM_H_INCLUDED

#include "options.h"

#if defined(FEATURE_SPEED_CONTROL)

class wpm {
public:
    wpm(byte potPin);
    void update(void);
    unsigned twitches(void) const { return m_twitches; }
    unsigned dot_twitches(void) const { return m_dotTwitches; }
    unsigned dash_twitches(void) const { return m_dashTwitches; }
    unsigned word_twitches(void) const { return m_wordTwitches; }

private:
    byte m_potPin;
    unsigned m_twitches;
    unsigned m_dotTwitches;
    unsigned m_dashTwitches;
    unsigned m_wordTwitches;
};

extern wpm *system_wpm;
void wpm_initialize(void);

#define WPM_INITIALIZE()    wpm_initialize()
#define WPM_UPDATE()        system_wpm->update()
#define WPM_TWITCHES()      system_wpm->twitches()
#define WPM_DOT_TWITCHES()  system_wpm->dot_twitches()
#define WPM_DASH_TWITCHES() system_wpm->dash_twitches()
#define WPM_WORD_TWITCHES() system_wpm->word_twitches()
#else // !defined(FEATURE_SPEED_CONTROL)
#define WPM_INITIALIZE()
#define WPM_UPDATE()
#define WPM_TWITCHES()      (MS_PER_TWITCH/DEFAULT_WPM)
#define WPM_DOT_TWITCHES()  (MS_PER_DOT/DEFAULT_WPM)
#define WPM_DASH_TWITCHES() (MS_PER_DASH/DEFAULT_WPM)
#define WPM_WORD_TWITCHES() (MS_PER_WORD/DEFAULT_WPM)

#endif // defined(FEATURE_SPEED_CONTROL)

#endif // !WPM_H_INCLUDED
