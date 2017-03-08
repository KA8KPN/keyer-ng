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
    m_wpmOffset = 0;
    m_wpmSuffix = ' ';
}

void wpm::update(void) {
    int potValue = 0;
    unsigned wpm;
 
    // The WPM is MIN_WPM + (MAX_WPM - MIN_WPM) * potValue / 1023; appropriately rounded
    potValue = analogRead(m_potPin);
    wpm = MIN_WPM + ((MAX_WPM - MIN_WPM) * potValue + ANALOG_IN_MAX/2) / ANALOG_IN_MAX;
    wpm += m_wpmOffset;
    if (MIN_WPM > wpm) {
	wpm = MIN_WPM;
    }
    else {
	if (MAX_WPM < wpm) {
	    wpm = MAX_WPM;
	}
    }
    DISPLAY_MANAGER_WPM(wpm, m_wpmSuffix);
    m_twitches = MS_PER_TWITCH/wpm;
    m_dotTwitches = MS_PER_DOT/wpm;
    m_dashTwitches = MS_PER_DASH/wpm;
    m_wordTwitches = MS_PER_WORD/wpm;
}


void wpm::update_suffix(void) {
    if (m_wpmOffset < 0) {
	m_wpmSuffix = '-';
    }
    else {
	if (m_wpmOffset > 0) {
	    m_wpmSuffix = '+';
	}
	else {
	    m_wpmSuffix = ' ';
	}
    }
}

void wpm::make_slower(int wpm) {
    m_wpmOffset -= wpm;
    if (-MAX_WPM > m_wpmOffset) {
	m_wpmOffset = -MAX_WPM;
    }
    update_suffix();
}

void wpm::make_faster(int wpm) {
    m_wpmOffset += wpm;
    if (MAX_WPM < m_wpmOffset) {
	m_wpmOffset = MAX_WPM;
    }
    update_suffix();
}


void wpm::offset(int o) {
    m_wpmOffset = o;
    if (MAX_WPM < m_wpmOffset) {
	m_wpmOffset = MAX_WPM;
    }
    else {
	if (-MAX_WPM > m_wpmOffset) {
	    m_wpmOffset = -MAX_WPM;
	}
    }
    update_suffix();
}

#endif // defined(FEATURE_SPEED_CONTROL)
