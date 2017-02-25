#include "config_manager.h"

#include "paddles.h"
#include "display.h"
#include "keying.h"
#include "memories.h"

#include <ctype.h>

config_manager system_config_manager;

void error_tone(void) {
    tone(SIDETONE, 100);
    delay(200);
    noTone(SIDETONE);
}


config_manager::config_manager(void): m_paddlesMode(MODE_PADDLE_NORMAL), m_isInCommandMode(false), m_currentXmitter(1), m_processing(false)  {
}

void config_manager::process_command(const char *command) {
    if (!m_processing) {
	switch(command[0]) {
	case 'N':
	    m_paddlesMode = PADDLES_REVERSE();
	    command_mode(false);
	    break;

	case 'O':
	    TOGGLE_SIDETONE_ENABLE();
	    command_mode(false);
	    break;

	case '0': case '1': case '2': case '3': case '4':
	    m_currentXmitter = command[0] - '0';
	    KEYING_SELECT_TRANSMITTER(m_currentXmitter);
	    command_mode(false);
	    break;

	case 'P':
	    if (isdigit(command[1])) {
		RECORD_MEMORY(atoi(command+1));
		m_processing = true;
	    }
	    else {
		error_tone();
	    }
	    break;

	default:
	    error_tone();
	    break;
	}
    }
}


static void two_beeps(bool first_is_low, bool second_is_low) {
    if (first_is_low) {
	tone(SIDETONE, 600);
    }
    else {
	tone(SIDETONE, 1200);
    }
    delay(100);
    noTone(SIDETONE);
    delay(50);
    if (second_is_low) {
	tone(SIDETONE, 600);
    }
    else {
	tone(SIDETONE, 1200);
    }
    delay(100);
    noTone(SIDETONE);
    delay(50);
}

void config_manager::memory_start_tones(void) {
    two_beeps(false, false);
}

void config_manager::memory_end_tones(void) {
    two_beeps(true, true);
}

void config_manager::command_mode(bool is_in_command_mode) {
    m_isInCommandMode = is_in_command_mode;
    DISPLAY_MANAGER_PROG_MODE(m_isInCommandMode);
    KEYING_COMMAND_MODE(m_isInCommandMode);
    if (is_in_command_mode) {
	two_beeps(true, false);
    }
    else {
	two_beeps(false, true);

	RECORD_MEMORY(0);
	m_processing = false;
    }
}


void config_manager_initialize(void) {
}
