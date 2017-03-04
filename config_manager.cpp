#include "config_manager.h"

#include "paddles.h"
#include "display.h"
#include "keying.h"
#include "memories.h"

#include <EEPROM.h>

#include <ctype.h>

config_manager system_config_manager;

#define MAGIC            0x5a0d

#define PADDLES_REVERSE_FLAG    0x01
#define SIDETONE_OFF_FLAG       0x02

#pragma pack(1)
typedef struct persistent_config {
    uint16_t magic;
    uint16_t size;
    uint8_t configFlags;
    uint8_t transmitter;
#ifdef FEATURE_MEMORIES
    uint8_t memories[MEMORY_SIZE];
#endif // FEATURE_MEMORIES
    uint16_t checksum;
} persistent_config_t;
#pragma pack()

void error_tone(void) {
    tone(SIDETONE, 100);
    delay(200);
    noTone(SIDETONE);
}

static persistent_config_t s_persistentConfig;

// returns TRUE if the checksum has changed
static bool update_checksum(persistent_config_t *conf) {
    uint16_t sum = 0;
    for (size_t i=0; i<((sizeof(persistent_config_t)/2)-1); ++i) {
	sum += ((uint16_t *)conf)[i];
    }

    if (~sum != conf->checksum) {
	conf->checksum = ~sum;
	return true;
    }
    return false;
}

// returns TRUE if the checksum is valid
static bool validate_checksum(persistent_config_t *conf) {
    uint16_t sum = 0;
    for (size_t i=0; i<(sizeof(persistent_config_t)/2); ++i) {
	sum += ((uint16_t *)conf)[i];
    }

    return sum == ~0U;
}

static void write_eeprom(bool force) {
    int max_address;

    max_address = sizeof(persistent_config_t);

    if (update_checksum(&s_persistentConfig) || force) {

	// Write the memory, but only if that works
	for (int i=0; i<max_address; ++i) {
	    if (EEPROM.read(i) != ((uint8_t *)&s_persistentConfig)[i]) {
		EEPROM.write(i, ((uint8_t *)&s_persistentConfig)[i]);
	    }
	}
    }
}


config_manager::config_manager(void): m_paddlesMode(MODE_PADDLE_NORMAL), m_isInCommandMode(false), m_currentXmitter(1), m_processing(false)  {
}

void config_manager::process_command(const char *command) {
    if (!m_processing) {
	switch(command[0]) {
	case 'N':
	    m_paddlesMode = PADDLES_REVERSE();
	    if (PADDLES_REVERSE_FLAG & s_persistentConfig.configFlags) {
		s_persistentConfig.configFlags &= ~PADDLES_REVERSE_FLAG;
	    }
	    else {
		s_persistentConfig.configFlags |= PADDLES_REVERSE_FLAG;
	    }
	    command_mode(false);
	    break;

	case 'O':
	    if (SIDETONE_OFF_FLAG & s_persistentConfig.configFlags) {
		s_persistentConfig.configFlags &= ~SIDETONE_OFF_FLAG;
	    }
	    else {
		s_persistentConfig.configFlags |= SIDETONE_OFF_FLAG;
	    }
	    TOGGLE_SIDETONE_ENABLE();
	    command_mode(false);
	    break;

	case '0': case '1': case '2': case '3': case '4':
	    m_currentXmitter = command[0] - '0';
	    s_persistentConfig.transmitter = m_currentXmitter;
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
    if (is_in_command_mode) {
	two_beeps(true, false);
    }
    else {
	if (m_isInCommandMode) {
	    two_beeps(false, true);
	}

	RECORD_MEMORY(0);
	m_processing = false;
	write_eeprom(false);
    }
    m_isInCommandMode = is_in_command_mode;
    DISPLAY_MANAGER_PROG_MODE(m_isInCommandMode);
    KEYING_COMMAND_MODE(m_isInCommandMode);
}


void config_manager_initialize(void) {
    // Attempt to read the config from the EEPROM
    int max_address;

    max_address = sizeof(persistent_config_t);

    for (int i=0; i<max_address; ++i) {
	((uint8_t *)&s_persistentConfig)[i] = EEPROM.read(i);
    }
    // If the magic is right, the size is right, and the checksum is right, then I'm good, otherwise initialize
    if ((MAGIC != s_persistentConfig.magic) ||
	(s_persistentConfig.size != sizeof(s_persistentConfig)) ||
	!validate_checksum(&s_persistentConfig)) {
	memset(&s_persistentConfig, 0, sizeof(s_persistentConfig));
	s_persistentConfig.magic = MAGIC;
	s_persistentConfig.size = sizeof(s_persistentConfig);
	s_persistentConfig.transmitter = 1;
	write_eeprom(true);
    }

    //
    MEMORIES_CONFIG(s_persistentConfig.memories);
    if (s_persistentConfig.configFlags & PADDLES_REVERSE_FLAG) {
	PADDLES_REVERSE();
    }
    if (s_persistentConfig.configFlags & SIDETONE_OFF_FLAG) {
	TOGGLE_SIDETONE_ENABLE();
    }
    KEYING_SELECT_TRANSMITTER(s_persistentConfig.transmitter);
}
