#include "options.h"

#ifdef FEATURE_MEMORIES

#include "Arduino.h"

#include "memories.h"
#include "wpm.h"
#include "display.h"
#include "morse_to_text.h"

memories system_memories;

uint8_t *s_memories;  // Actually an array of MEMORY_SIZE that is owned by the configuration manager

void memories_initialize(void) {
}

void memories::config(uint8_t *memories) {
    s_memories = memories;
    int idxptr = 1;
    int memptr = 1;

    m_index[0] = 0;
    while (idxptr < MAX_MEMORIES) {
	if (0 == s_memories[memptr]) {
	    m_index[idxptr] = memptr;
	    ++idxptr;
	}
	++memptr;
    }
}


memories::memories() {
    m_mptr = -1;
    m_lastByteTime = 0;
    m_memRecording = 0;
    m_bytesFree = 0;
    m_longPause = false;
}


void memories::play_memory(uint8_t m) {
    if ((0 < m) && (MAX_MEMORIES >= m)) {
	m_memory = m;
	m_mptr = m_index[m-1];
    }
}


void memories::record_memory(uint8_t m) {
    // If m is nonzero, need to set up for recording.  I think I want to keep the memories in strictly
    // ascending order, so I need to make room in the middle if it is in the middle.
    // If m is zero, then I need to finish recording, if a recording was being made.
    // I think I want to keep the memories tightly packed, so I need to put the stop byte in, then I
    // need to move the data from the top down, squeezing out the empty space.

    // At first, I will have one memory and I will record it.  It will always start at offset 0.
    if (0 == m) {
	if (0 != m_memRecording) {
	    CONFIG_MANAGER_MEM_END_TONES();
	    s_memories[m_recordPtr] = 0;

	    if (12 != m_memRecording) {
		uint16_t offset;
		// Now, I have to move everything down from m_index[m_memRecording] to m_recordPtr+1 unless, I was doing memory 12.
		memmove(s_memories+m_recordPtr+1, s_memories+m_index[m_memRecording], MEMORY_SIZE-m_index[m_memRecording]+1);
		offset = m_index[m_memRecording] - m_recordPtr - 1;
		for (int i=m_memRecording; i<12; ++i) {
		    m_index[i] -= offset;
		}
	    }
	}

	KEYING_RECORD_MODE(false);
	DISPLAY_MANAGER_CLEAR_NUMBER();
    }
    else {
	if (13 > m) {
	    CONFIG_MANAGER_MEM_START_TONES();
	    // Step, the first.  Find the space to begin recording.  This will be where the current memory begins.
	    m_recordPtr = m_index[m-1];
	    if (m_recordPtr < MEMORY_SIZE) {
		// Step, the second.  I need to move everything above memory "m" to the top of the memory space
		// Memory 12 is special.  I don't move anything if I'm recording memory m.  That means I have to
		// figure out the move for memories 1 through 11
		if (12 > m) {
		    uint16_t top;
		    top = m_index[11];
		    while (0 != s_memories[top]) {
			++top;
		    }
		    // at this point, "top" points to the terminator for memory 11.  I need to move that memory as
		    // a block and update the pointers in m_index
		    memmove(s_memories+MEMORY_SIZE+m_index[m]-top-1, s_memories+m_index[m], 1+top-m_index[m]);
		    for (int i=m; i<12; ++i) {
			m_index[i] += MEMORY_SIZE-top-1;
		    }
		    // Step, the last.  I need to calculate the number of bytes free.
		    m_bytesFree = m_index[m] - m_recordPtr - 1;
		}
		else {
		    m_bytesFree = MEMORY_SIZE - m_recordPtr - 1;
		}
		KEYING_RECORD_MODE(true);
	    }
	    m_index[m-1] = m_recordPtr;
	    DISPLAY_MANAGER_NUMBER(m_bytesFree);
	}
	else {
	    // Invalid memory number.  Give it up as a bad job
	    KEYING_RECORD_MODE(false);
	}
    }
    m_lastByteTime = 0;
    m_memRecording = m;
    m_longPause = true;
    m_beginningOfWord = m_recordPtr;
}

void memories::record_element(bool is_key_down) {
    // Procedure is this:  Take the time difference between the last transition and multiply by the twitches
    // per ms to get twitches, and then encode that.  If we've run out of space, then stop recording, otherwise
    // advance the counter.
    unsigned long now = millis();

    // char buffer[10];
    // sprintf(buffer, "%8d", m_bytesFree);
    // system_display_manager->m_display->setCursor(12, 2);
    // system_display_manager->m_display->write((const uint8_t*) buffer, 8);
    if (0 < m_bytesFree) {
	if (is_key_down) {
	    // Was up, now it's down.  If this is the first key down since recording started, I don't do
	    // anything except record the time now, which is done elsewhere.  If I'm in the middle of the
	    // recording, I record the silence that just got done.
	    if (0 != m_lastByteTime) {
		unsigned long diff;
		diff = (WPM_TWITCHES()/2 + now - m_lastByteTime) / WPM_TWITCHES();
		// If it's a word pause, then a macro may come next.  This code knows from nothing detecting macros
		// but I need to record the fact that I have a long pause so that I can mark the place that comes
		// immediately after the long pause
		// serial_log("Diff is %lu\r\n", diff);
		if (diff > 70) {
		    m_longPause = true;
		}
		while ((0 < m_bytesFree) && (diff > 62)) {
		    s_memories[m_recordPtr++] = 0xbf;
		    --m_bytesFree;
		    diff -= 63;
		}
		if (0 < m_bytesFree) {
		    s_memories[m_recordPtr++] = 0x80 | (0x3f & diff);
		    --m_bytesFree;
		}
	    }
	}
	else {
	    // Was down, now it's up.  Record the time that the key was down.
	    unsigned long diff;
	    if (m_longPause) {
		m_longPause = false;
		m_beginningOfWord = m_recordPtr;
	    }
	    diff = (WPM_TWITCHES()/2 + now - m_lastByteTime) / WPM_TWITCHES();
	    while ((0 < m_bytesFree) && (diff > 62)) {
		s_memories[m_recordPtr++] = 0xff;
		--m_bytesFree;
		diff -= 63;
	    }
	    if (0 < m_bytesFree) {
		s_memories[m_recordPtr++] = 0xc0 | (0x3f & diff);
		--m_bytesFree;
	    }
	}
    }

    m_lastByteTime = millis();
    DISPLAY_MANAGER_NUMBER(m_bytesFree);
}


void memories::record_special_element(uint8_t macro_number) {
    // serial_log("The macro number is %#02x, m_recordPtr = %d, m_beginningOfWord = %d\r\n", macro_number, m_recordPtr, m_beginningOfWord);
    m_bytesFree += m_recordPtr - m_beginningOfWord;
    s_memories[m_beginningOfWord] = 0x40 | (0x3f & macro_number);
    m_recordPtr = m_beginningOfWord + 1;
}

input_mode_t memories::update(unsigned long now, input_mode_t mode) {
    if ((MODE_MEMORY != mode) && (-1 < m_mptr)) {
	mode = MODE_MEMORY;
	DISPLAY_MANAGER_INPUT_SOURCE(mode, m_memory);
	m_nextByteTime = now-1;
    }
    if ((MODE_MEMORY == mode) && (m_nextByteTime < now)) {
	// serial_log("s_memories[%d] = %#02x\r\n", m_mptr, s_memories[m_mptr]);
	switch (s_memories[m_mptr] >> 6) {
	case 2:
	    m_nextByteTime = now + WPM_TWITCHES() * (0x3f & s_memories[m_mptr]);
	    TRANSMITTER_KEY_UP();
	    if (15 < (0x3f & s_memories[m_mptr])) {
		MORSE_TO_TEXT_UPDATE(CharSpace);
		if (40 < (0x3f & s_memories[m_mptr])) {
		    MORSE_TO_TEXT_UPDATE(WordSpace);
		}
	    }
	    m_mptr++;
	    break;

	case 3:
	    if (15 < (0x3f & s_memories[m_mptr])) {
		MORSE_TO_TEXT_UPDATE(Dah);
	    }
	    else {
		MORSE_TO_TEXT_UPDATE(Dit);
	    }
	    m_nextByteTime = now + WPM_TWITCHES() * (0x3f & s_memories[m_mptr]) + TRANSMITTER_KEY_DOWN();
	    m_mptr++;
	    break;

	case 1:
	    // Invoke the macro.  Ummm, how do I do that?
	    m_mptr++;
	    break;

	default:
	    TRANSMITTER_KEY_UP();
	    mode = CONFIG_MANAGER_PADDLES_MODE();
	    DISPLAY_MANAGER_INPUT_SOURCE(mode, 0);
	    m_mptr = -1;
	    MORSE_TO_TEXT_UPDATE(CharSpace);
	    MORSE_TO_TEXT_UPDATE(WordSpace);
	    break;
	}
    }

    return mode;
}

#endif // FEATURE_MEMORIES
