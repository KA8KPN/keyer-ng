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
    memories(void);
    input_mode_t update(unsigned long now, input_mode_t mode);
    void play_memory(uint8_t which_memory_to_play);
    // Memory is between 1 and the maximum number of memories, inclusive
    // record_memory(0) means stop recording
    void record_memory(uint8_t which_memory_to_record);
    void record_element(bool is_key_down);
    void record_special_element(uint8_t macro_number);
    void config(uint8_t *memories);
    bool is_recording(void) const { return 0 != m_memRecording; }

private:
    uint16_t m_index[MAX_MEMORIES];
    int16_t m_mptr;
    uint8_t m_memory;
    unsigned long m_nextByteTime;
    unsigned long m_lastByteTime;
    uint8_t m_memRecording;
    uint16_t m_bytesFree;
    uint16_t m_recordPtr;
    uint16_t m_beginningOfWord;
    bool     m_longPause;
};

extern memories system_memories;

#define MEMORIES_INITIALIZE() memories_initialize()
#define MEMORIES_CONFIG(memories)            system_memories.config(memories)
#define MEMORIES_UPDATE(now, keyer_mode)     system_memories.update(now, keyer_mode)
#define PLAY_MEMORY(n)        system_memories.play_memory(n)
#define RECORD_MEMORY(n)      system_memories.record_memory(n)
#define RECORD_ELEMENT(is_key_down) system_memories.record_element(is_key_down)
#define RECORD_SPECIAL_ELEMENT(macro)         system_memories.record_special_element(macro)
#define MEMORY_RECORDING()                    system_memories.is_recording()
#else // ! FEATURE_MEMORIES
#define MEMORIES_INITIALIZE()
#define MEMORIES_UPDATE(now, keyer_mode) CONFIG_MANAGER_PADDLES_MODE()
#define PLAY_MEMORY(n)        ((void) n)
#define RECORD_MEMORY(n)        ((void) n)
#define RECORD_ELEMENT(is_key_up) ((void)is_key_down)
#define RECORD_SPECIAL_ELEMENT(macro) ((void)macro)
#define MEMORIES_CONFIG(memories) ((void)memories)
#define MEMORY_RECORDING()                    (false)
#endif // ! FEATURE_MEMORIES

#endif // MEMORIES_H_INCLUDED

    
