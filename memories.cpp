#include "options.h"

#ifdef FEATURE_MEMORIES

#include "Arduino.h"

#include "memories.h"
#include "wpm.h"
#include "display.h"
#include "morse_to_text.h"

memories *system_memories = NULL;

static uint8_t s_memories[MEMORY_SIZE];
#if 0
static uint8_t initialization[MEMORY_SIZE] = {
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xca, 0x9e,                                                 // "E"
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e,                         // "O"
    0xca, 0x8a, 0xde, 0x8a, 0xca, 0x9e,                         // "R"
    0xde, 0x8a, 0xca, 0x8a, 0xde, 0x8a, 0xde, 0x9e,             // "Y"
    0xa8,
    0xca, 0x8a, 0xde, 0x8a, 0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e, // "1"
    0x00,
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xca, 0x9e,                                                 // "E"
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e,                         // "O"
    0xca, 0x8a, 0xde, 0x8a, 0xca, 0x9e,                         // "R"
    0xde, 0x8a, 0xca, 0x8a, 0xde, 0x8a, 0xde, 0x9e,             // "Y"
    0xa8,
    0xca, 0x8a, 0xca, 0x8a, 0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e, // "2"
    0x00,
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xca, 0x9e,                                                 // "E"
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e,                         // "O"
    0xca, 0x8a, 0xde, 0x8a, 0xca, 0x9e,                         // "R"
    0xde, 0x8a, 0xca, 0x8a, 0xde, 0x8a, 0xde, 0x9e,             // "Y"
    0xa8,
    0xca, 0x8a, 0xde, 0x8a, 0xca, 0x8a, 0xde, 0x8a, 0xde, 0x9e, // "3"
    0x00,
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xca, 0x9e,                                                 // "E"
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e,                         // "O"
    0xca, 0x8a, 0xde, 0x8a, 0xca, 0x9e,                         // "R"
    0xde, 0x8a, 0xca, 0x8a, 0xde, 0x8a, 0xde, 0x9e,             // "Y"
    0xa8,
    0xca, 0x8a, 0xca, 0x8a, 0xca, 0x8a, 0xca, 0x8a, 0xde, 0x9e, // "4"
    0x00,
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xca, 0x9e,                                                 // "E"
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e,                         // "O"
    0xca, 0x8a, 0xde, 0x8a, 0xca, 0x9e,                         // "R"
    0xde, 0x8a, 0xca, 0x8a, 0xde, 0x8a, 0xde, 0x9e,             // "Y"
    0xa8,
    0xca, 0x8a, 0xca, 0x8a, 0xca, 0x8a, 0xca, 0x8a, 0xca, 0x9e, // "5"
    0x00,
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xca, 0x9e,                                                 // "E"
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e,                         // "O"
    0xca, 0x8a, 0xde, 0x8a, 0xca, 0x9e,                         // "R"
    0xde, 0x8a, 0xca, 0x8a, 0xde, 0x8a, 0xde, 0x9e,             // "Y"
    0xa8,
    0xde, 0x8a, 0xca, 0x8a, 0xca, 0x8a, 0xca, 0x8a, 0xca, 0x9e, // "6"
    0x00,
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xca, 0x9e,                                                 // "E"
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e,                         // "O"
    0xca, 0x8a, 0xde, 0x8a, 0xca, 0x9e,                         // "R"
    0xde, 0x8a, 0xca, 0x8a, 0xde, 0x8a, 0xde, 0x9e,             // "Y"
    0xa8,
    0xde, 0x8a, 0xde, 0x8a, 0xca, 0x8a, 0xca, 0x8a, 0xca, 0x9e, // "7"
    0x00,
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xca, 0x9e,                                                 // "E"
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e,                         // "O"
    0xca, 0x8a, 0xde, 0x8a, 0xca, 0x9e,                         // "R"
    0xde, 0x8a, 0xca, 0x8a, 0xde, 0x8a, 0xde, 0x9e,             // "Y"
    0xa8,
    0xde, 0x8a, 0xde, 0x8a, 0xde, 0x8a, 0xca, 0x8a, 0xca, 0x9e, // "8"
    0x00,
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xca, 0x9e,                                                 // "E"
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e,                         // "O"
    0xca, 0x8a, 0xde, 0x8a, 0xca, 0x9e,                         // "R"
    0xde, 0x8a, 0xca, 0x8a, 0xde, 0x8a, 0xde, 0x9e,             // "Y"
    0xa8,
    0xde, 0x8a, 0xde, 0x8a, 0xde, 0x8a, 0xde, 0x8a, 0xca, 0x9e, // "9"
    0x00,
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xca, 0x9e,                                                 // "E"
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e,                         // "O"
    0xca, 0x8a, 0xde, 0x8a, 0xca, 0x9e,                         // "R"
    0xde, 0x8a, 0xca, 0x8a, 0xde, 0x8a, 0xde, 0x9e,             // "Y"
    0xa8,
    0xca, 0x8a, 0xde, 0x8a, 0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e, // "1"
    0xde, 0x8a, 0xde, 0x8a, 0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e, // "0"
    0x00,
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xca, 0x9e,                                                 // "E"
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e,                         // "O"
    0xca, 0x8a, 0xde, 0x8a, 0xca, 0x9e,                         // "R"
    0xde, 0x8a, 0xca, 0x8a, 0xde, 0x8a, 0xde, 0x9e,             // "Y"
    0xa8,
    0xca, 0x8a, 0xde, 0x8a, 0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e, // "1"
    0xca, 0x8a, 0xde, 0x8a, 0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e, // "1"
    0x00,
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xca, 0x9e,                                                 // "E"
    0xde, 0x8a, 0xde, 0x9e,                                     // "M"
    0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e,                         // "O"
    0xca, 0x8a, 0xde, 0x8a, 0xca, 0x9e,                         // "R"
    0xde, 0x8a, 0xca, 0x8a, 0xde, 0x8a, 0xde, 0x9e,             // "Y"
    0xa8,
    0xca, 0x8a, 0xde, 0x8a, 0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e, // "1"
    0xca, 0x8a, 0xca, 0x8a, 0xde, 0x8a, 0xde, 0x8a, 0xde, 0x9e, // "2"
    0x00
};
#endif // 0

void memories_initialize(void) {
    system_memories = new memories(12);
}

memories::memories(int n) {
    #if 0
    // Okay, to begin with, I'm going to program each memory with "memory <X>" where <X> will be the memory
    // number from 1 12
    // So, the values in each memory will begin with
    // 0xde 0x8a 0xde 0x9e                     "M"
    // 0xca 0x9e                               "E"
    // 0xde 0x8a 0xde 0x9e                     "M"
    // 0xde 0x8a 0xde 0x8a 0xde 0x9e           "O"
    // 0xca 0x8a 0xde 0x8a 0xca 0x9e           "R"
    // 0xde 0x8a 0xca 0x8a 0xde 0x8a 0xde 0x9e "Y"

    // And the digits
    // 0xde 0x8a 0xde 0x8a 0xde 0x8a 0xde 0x8a 0xde 0x9e "0"
    // 0xca 0x8a 0xde 0x8a 0xde 0x8a 0xde 0x8a 0xde 0x9e "1"
    // 0xca 0x8a 0xca 0x8a 0xde 0x8a 0xde 0x8a 0xde 0x9e "2"
    // 0xca 0x8a 0xde 0x8a 0xca 0x8a 0xde 0x8a 0xde 0x9e "3"
    // 0xca 0x8a 0xca 0x8a 0xca 0x8a 0xca 0x8a 0xde 0x9e "4"
    // 0xca 0x8a 0xca 0x8a 0xca 0x8a 0xca 0x8a 0xca 0x9e "5"
    // 0xde 0x8a 0xca 0x8a 0xca 0x8a 0xca 0x8a 0xca 0x9e "6"
    // 0xde 0x8a 0xde 0x8a 0xca 0x8a 0xca 0x8a 0xca 0x9e "7"
    // 0xde 0x8a 0xde 0x8a 0xde 0x8a 0xca 0x8a 0xca 0x9e "8"
    // 0xde 0x8a 0xde 0x8a 0xde 0x8a 0xde 0x8a 0xca 0x9e "9"

    // And they all end with 0x00.  In addition there will be an additional 40 twitch times between
    // the Y and the digit, which will be encoded with 0xa8
    // 10011110
    // 00001010
    // 10101000

    // So, the first 9 memories are all going to be 42 bytes long and the last three will all be 52 bytes long
    m_index[0] = 0;
    m_index[1] = 42;
    m_index[2] = 84;
    m_index[3] = 126;
    m_index[4] = 168;
    m_index[5] = 210;
    m_index[6] = 252;
    m_index[7] = 294;
    m_index[8] = 336;
    m_index[9] = 378;
    m_index[10] = 430;
    m_index[11] = 482;

    for (i=0; i<MEMORY_SIZE; ++i) {
	s_memories[i] = initialization[i];
	// s_memories[i] = 0;
    }
#endif // 0

    memset(s_memories, 0, MEMORY_SIZE);
    m_index[0] = 0;
    m_index[1] = 0;
    m_index[2] = 0;
    m_index[3] = 0;
    m_index[4] = 0;
    m_index[5] = 0;
    m_index[6] = 0;
    m_index[7] = 0;
    m_index[8] = 0;
    m_index[9] = 0;
    m_index[10] = 0;
    m_index[11] = 0;
    m_mptr = -1;
    m_lastByteTime = 0;
    m_memRecording = 0;
    m_bytesFree = 0;
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
    if ((0 == m) && (0 != m_memRecording)) {
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

	KEYING_RECORD_MODE(false);
    }
    else {
	// Step, the first.  Find the space to begin recording.  This will only be different if "m" is
	// greater than 1 and if m-1 is the same as m.
	if (m > 1) {
	    if (m_index[m-2] == m_index[m-1]) {
		m_recordPtr = m_index[m-2];
		while (0 != s_memories[m_recordPtr]) {
		    ++m_recordPtr;
		}
		++m_recordPtr; // I'm looking for the character past the last one
	    }
	    else {
		m_recordPtr = m_index[m-1];
	    }
	}
	else {
	    m_recordPtr = 0;
	}
	if (m_recordPtr < MEMORY_SIZE) {
	    // Step, the second.  I need to move everything above memory "m" to the top of the memory space
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
		m_bytesFree = MEMORY_SIZE - top + m_index[m] - m_recordPtr - 2;
	    }
	    else {
		m_bytesFree = MEMORY_SIZE - m_recordPtr - 1;
	    }
	    KEYING_RECORD_MODE(true);
	}
	m_index[m-1] = m_recordPtr;
    }
    m_lastByteTime = 0;
    m_memRecording = m;
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
		while ((0 < m_bytesFree) && (diff > 63)) {
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
	    diff = (WPM_TWITCHES()/2 + now - m_lastByteTime) / WPM_TWITCHES();
	    while ((0 < m_bytesFree) && (diff > 63)) {
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
}


input_mode_t memories::update(unsigned long now, input_mode_t mode) {
    if ((MODE_MEMORY != mode) && (-1 < m_mptr)) {
	mode = MODE_MEMORY;
	DISPLAY_MANAGER_INPUT_SOURCE(mode, m_memory);
	m_nextByteTime = now-1;
    }
    if ((MODE_MEMORY == mode) && (m_nextByteTime < now)) {
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
