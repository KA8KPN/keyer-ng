#ifndef CONFIG_MANAGER_H_INCLUDED
#define CONFIG_MANAGER_H_INCLUDED

#include <Arduino.h>

#include "keyer.h"

class config_manager {
public:
    config_manager(void);
    void process_command(uint16_t command);
    input_mode_t paddles_mode(void) const { return m_paddlesMode; }
    bool program_mode(void) const { return m_isInProgramMode; }
    void program_mode(bool is_in_program_mode);
    uint8_t xmitter(void) const { return m_currentXmitter; }

private:
    input_mode_t m_paddlesMode;
    bool m_isInProgramMode;
    uint8_t m_currentXmitter;
};


extern config_manager *system_config_manager;
void config_manager_initialize(void);
#define CONFIG_MANAGER_INITIALIZE() config_manager_initialize()
#define CONFIG_MANAGER_PROCESS_COMMAND(command) system_config_manager->process_command(command)
#define CONFIG_MANAGER_PADDLES_MODE()  system_config_manager->paddles_mode()
#define CONFIG_MANAGER_GET_PROGRAM_MODE()  system_config_manager->program_mode()
#define CONFIG_MANAGER_SET_PROGRAM_MODE(b) system_config_manager->program_mode(b)
#define CONFIG_MANAGER_XMITTER()           system_config_manager->xmitter()

#endif // CONFIG_MANAGER_H_INCLUDED
