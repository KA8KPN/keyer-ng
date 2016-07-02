#include "options.h"

#if defined(FEATURE_SPEED_CONTROL)

#include <Arduino.h>

#include "wpm.h"
#include "display.h"

#include "keyer.h"

wpm *system_wpm = NULL;

void wpm_initialize(void) {
    system_wpm = new wpm(WPM_POT_PORT);
}

wpm::wpm(byte potPin) : m_potPin(potPin) {
}

void wpm::update(void) {
    int potValue = 0;
    unsigned wpm;
 
    // The WPM is MIN_WPM + (MAX_WPM - MIN_WPM) * potValue / 1024; appropriately rounded
    potValue = analogRead(m_potPin);
    wpm = MIN_WPM + ((MAX_WPM - MIN_WPM) * potValue + 512L) / 1024L;
    DISPLAY_MANAGER_WPM(wpm);
    m_dotTwitches = MS_PER_DOT/wpm;
    m_dashTwitches = MS_PER_DASH/wpm;
    m_wordTwitches = MS_PER_WORD/wpm;
}

#endif // defined(FEATURE_SPEED_CONTROL)
