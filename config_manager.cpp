#include "config_manager.h"

#include "paddles.h"
#include "display.h"
#include "keying.h"

config_manager system_config_manager;

config_manager::config_manager(void): m_paddlesMode(MODE_PADDLE_NORMAL), m_isInCommandMode(false), m_currentXmitter(1)  {
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

void config_manager::command_mode(bool is_in_command_mode) {
    m_isInCommandMode = is_in_command_mode;
    DISPLAY_MANAGER_PROG_MODE(m_isInCommandMode);
    KEYING_COMMAND_MODE(m_isInCommandMode);
}


void config_manager_initialize(void) {
}
