#ifndef CONFIG_MANAGER_H_INCLUDED
#define CONFIG_MANAGER_H_INCLUDED

#include <Arduino.h>

#include "keyer.h"

class config_manager {
public:
    config_manager(void);
    void process_command(uint16_t command);
    keyer_mode_t paddles_mode(void) const { return m_paddlesMode; }

private:
    keyer_mode_t m_paddlesMode;
};


extern config_manager *system_config_manager;
void config_manager_initialize(void);
#define CONFIG_MANAGER_INITIALIZE() config_manager_initialize()
#define CONFIG_MANAGER_PROCESS_COMMAND(command) system_config_manager->process_command(command)
#define CONFIG_MANAGER_PADDLES_MODE() system_config_manager->paddles_mode()

#endif // CONFIG_MANAGER_H_INCLUDED
