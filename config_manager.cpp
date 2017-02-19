#include "config_manager.h"

#include "paddles.h"
#include "display.h"
#include "keying.h"
#include "memories.h"

#include <ctype.h>

config_manager system_config_manager;

config_manager::config_manager(void): m_paddlesMode(MODE_PADDLE_NORMAL), m_isInCommandMode(false), m_currentXmitter(1)  {
}

void config_manager::process_command(const char *command) {
    switch(command[0]) {
    case 'N':
	m_paddlesMode = PADDLES_REVERSE();
	break;

    case 'O':
	TOGGLE_SIDETONE_ENABLE();
	break;

    case '0': case '1': case '2': case '3': case '4':
	m_currentXmitter = command[0] - '0';
	KEYING_SELECT_TRANSMITTER(m_currentXmitter);
	break;

    case 'P':
	if (isdigit(command[1])) {
	    RECORD_MEMORY(atoi(command+1));
	    break;
	}
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
