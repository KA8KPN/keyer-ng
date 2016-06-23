#include "Arduino.h"

#include "wpm.h"

#include "keyer.h"

wpm::wpm(byte potPin, display *display_manager) : m_potPin(potPin), m_displayManager(display_manager) {
}

void wpm::update(void) {
    int potValue = 0;
    unsigned wpm;
 
    // The WPM is MIN_WPM + (MAX_WPM - MIN_WPM) * potValue / 1024; appropriately rounded
    potValue = analogRead(m_potPin);
    wpm = MIN_WPM + ((MAX_WPM - MIN_WPM) * potValue + 512L) / 1024L;
    m_displayManager->wpm(wpm);
    m_dotTwitches = MS_PER_DOT/wpm;
    m_dashTwitches = MS_PER_DASH/wpm;
    m_wordTwitches = MS_PER_WORD/wpm;
}

