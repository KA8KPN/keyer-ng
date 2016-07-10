#include "config_manager.h"

#include "paddles.h"
#include "display.h"
#include "keying.h"

config_manager *system_config_manager = NULL;

config_manager::config_manager(void): m_paddlesMode(MODE_PADDLE_NORMAL), m_isInProgramMode(false), m_currentXmitter(1)  {
}

void config_manager::process_command(uint16_t command) {
    switch(command) {
    case 'N':
	m_paddlesMode = PADDLES_REVERSE();
	break;

    case 'O':
	TOGGLE_SIDETONE_ENABLE();
	break;

    case '0': case '1': case '2': case '3': case '4':
	m_currentXmitter = command - '0';
	KEYING_SELECT_TRANSMITTER(m_currentXmitter);
	break;

    default:
	break;
    }
}

void config_manager::program_mode(bool is_in_program_mode) {
    m_isInProgramMode = is_in_program_mode;
    DISPLAY_MANAGER_PROG_MODE(m_isInProgramMode);
    KEYING_PROGRAM_MODE(m_isInProgramMode);
}


void config_manager_initialize(void) {
    system_config_manager = new config_manager();
}
