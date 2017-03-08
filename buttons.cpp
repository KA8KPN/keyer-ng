#include "options.h"

#if defined(FEATURE_BUTTONS)

#include <Arduino.h>

#include "buttons.h"
#include "text_to_morse.h"
#include "config_manager.h"
#include "memories.h"

buttons *system_buttons = NULL;

void buttons_initialize(void) {
    system_buttons = new buttons(BUTTONS_AI_PORT);
}

buttons::buttons(byte aiPin) : m_aiPin(aiPin), m_oldButton(~0) {
}

// 0   - button 0 (--)
// 91  - button 1 (91)
// 169 - button 2 (78)
// 234 - button 3 (65)
// 291 - button 4 (63)
// 340 - button 5 (49)
// 383 - button 6 (43)
// 420 - button 7 (37)
// 454 - button 8 (34)
// 484 - button 9 (30)
// 511 - button 10 (27)
// 535 - button 11 (24)
// 557 - button 12 (22)

/*
 * From the schematic, the analog input value for a given button press is 
 *     aiv = b * 1023 / (10+b)
 * where "b" is the button number (0 to 12, where 0 is the program button)
 *
 * Since I want to know b given aiv, I have to exercise my algebra skills,
 * which gets me
 *     b = 10*aiv/(1023-aiv)
 * I will add half of 1023-aiv to the numerator of that, in order to cause
 * it to round properly, which gives me the formula I put here
 */
void buttons::update(void) {
    int potValue = 0;
    int button;

    potValue = analogRead(m_aiPin);
    if ((ANALOG_IN_MAX - 100) > potValue) {
        button = (10*potValue + (ANALOG_IN_MAX-potValue)/2) / (ANALOG_IN_MAX - potValue);
	if (button != m_oldButton) {
	    m_oldButton = button;
	    switch(button) {
	    case 0:
                CONFIG_MANAGER_SET_COMMAND_MODE(!CONFIG_MANAGER_GET_COMMAND_MODE());
		break;

	    case 1: case 2: case 3: case 4:
		PLAY_MEMORY(button);
		break;

	    case 5:
		if (CONFIG_MANAGER_GET_COMMAND_MODE()) {
		    WPM_MAKE_FASTER(5);
		}
		else {
		    PLAY_MEMORY(button);
		}
		break;

	    case 6:
		if (CONFIG_MANAGER_GET_COMMAND_MODE()) {
		    WPM_MAKE_FASTER(1);
		}
		else {
		    PLAY_MEMORY(button);
		}
		break;

	    case 7: case 8: case 9: case 10:
		PLAY_MEMORY(button);
		break;

	    case 11:
		if (CONFIG_MANAGER_GET_COMMAND_MODE()) {
		    WPM_MAKE_SLOWER(5);
		}
		else {
		    PLAY_MEMORY(button);
		}
		break;

	    case 12:
		if (CONFIG_MANAGER_GET_COMMAND_MODE()) {
		    WPM_MAKE_SLOWER(1);
		}
		else {
		    PLAY_MEMORY(button);
		}
		break;

	    default:
		TEXT_TO_MORSE_ADD_TO_BUFFER('?');
		break;
	    }
        }
    }
    else {
	m_oldButton = ~0;
    }
}

#endif // defined(FEATURE_SPEED_CONTROL)
