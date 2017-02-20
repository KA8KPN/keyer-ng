#include "config_manager.h"

#include "paddles.h"
#include "display.h"
#include "keying.h"
#include "memories.h"

#include <ctype.h>

config_manager system_config_manager;

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
	    RECORD_MEMORY(1);
	    m_processing = true;
#if 0
	    KEYING_RECORD_MODE(true);
	    if (isdigit(command[1])) {
	    }
#endif // 0
	    break;

	default:
	    break;
	}
    }
}

void config_manager::command_mode(bool is_in_command_mode) {
    m_isInCommandMode = is_in_command_mode;
    DISPLAY_MANAGER_PROG_MODE(m_isInCommandMode);
    KEYING_COMMAND_MODE(m_isInCommandMode);
    if (!is_in_command_mode) {
	RECORD_MEMORY(0);
	m_processing = false;
    }
}


void config_manager_initialize(void) {
}
