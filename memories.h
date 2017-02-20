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
    // Memory is between 1 and the maximum number of memories, inclusive
    // record_memory(0) means stop recording
    void record_memory(uint8_t which_memory_to_record);
    void record_element(bool is_key_down);

private:
    uint16_t m_index[12];
    int16_t m_mptr;
    uint8_t m_memory;
    unsigned long m_nextByteTime;
    unsigned long m_lastByteTime;
    uint8_t m_memRecording;
    uint16_t m_bytesFree;
    uint16_t m_recordPtr;
};

extern memories *system_memories;

#define MEMORIES_INITIALIZE() memories_initialize();
#define MEMORIES_UPDATE(now, keyer_mode)     system_memories->update(now, keyer_mode)
#define PLAY_MEMORY(n)        system_memories->play_memory(n)
#define RECORD_MEMORY(n)      system_memories->record_memory(n)
#define RECORD_ELEMENT(is_key_down) system_memories->record_element(is_key_down)
#else // ! FEATURE_MEMORIES
#define MEMORIES_INITIALIZE()
#define MEMORIES_UPDATE(now, keyer_mode) CONFIG_MANAGER_PADDLES_MODE()
#define PLAY_MEMORY(n)        ((void) n)
#define RECORD_MEMORY(n)        ((void) n)
#define RECORD_ELEMENT(is_key_up) ((void)is_key_down)
#endif // ! FEATURE_MEMORIES

#endif // MEMORIES_H_INCLUDED

    
