#ifndef MEMORIES_H_INCLUDED
#define MEMORIES_H_INCLUDED

#include "options.h"

#ifdef FEATURE_MEMORIES

#include "keying.h"
#include "keyer.h"
#include "config_manager.h"

void memories_initialize(void);
#define MEMORY_SIZE 1000
#define MAX_MEMORIES 12

class memories {
public:
    memories(int number_of_memories);
    input_mode_t update(unsigned long now, input_mode_t mode);
    void play_memory(uint8_t which_memory_to_play);

private:
    uint16_t m_index[12];
    int16_t m_mptr;
    unsigned long m_nextByteTime;
};

extern memories *system_memories;

#define MEMORIES_INITIALIZE() memories_initialize();
#define MEMORIES_UPDATE(now, keyer_mode)     system_memories->update(now, keyer_mode);
#define PLAY_MEMORY(n)        system_memories->play_memory(n);
#else // ! FEATURE_MEMORIES
#define MEMORIES_INITIALIZE()
#define MEMORIES_UPDATE(now, keyer_mode) CONFIG_MANAGER_PADDLES_MODE()
#define PLAY_MEMORY(n)        ((void) n)
#endif // ! FEATURE_MEMORIES

#endif // MEMORIES_H_INCLUDED

    
