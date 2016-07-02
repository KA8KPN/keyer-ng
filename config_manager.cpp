#include "config_manager.h"

#include "paddles.h"

config_manager *system_config_manager = NULL;

config_manager::config_manager(void): m_paddlesMode(MODE_PADDLE_NORMAL)  {
}

void config_manager::process_command(uint16_t command) {
    switch(command) {
    case 'N':
	m_paddlesMode = PADDLES_REVERSE();
	break;

    default:
	break;
    }
}


void config_manager_initialize(void) {
    system_config_manager = new config_manager();
}
