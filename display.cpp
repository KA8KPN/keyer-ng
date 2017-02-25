#include <Arduino.h>

#include "display.h"

#ifdef LCD_DISPLAY

#include "config_manager.h"

#if defined(DISPLAY_LARGE)
#define ROWS     4
#define COLUMNS 20

#define KEYING_MODE_ROW 0
#define KEYING_MODE_COL 0
#define KEYING_MODE_WIDTH 8

#define PROG_MODE_ROW 0
#define PROG_MODE_COL 9
#define PROG_MODE_WIDTH 4

#define SPEED_ROW 0
#define SPEED_COL 15
#define SPEED_WIDTH 5

#define XMITTER_ROW 1
#define XMITTER_COL 0
#define XMITTER_WIDTH 11

#define SIDETONE_ROW 1
#define SIDETONE_COL 11
#define SIDETONE_WIDTH 9

#define INPUT_ROW 2
#define INPUT_COL 0
#define INPUT_WIDTH 11

#define NUMBER_ROW 2
#define NUMBER_COL 11
#define NUMBER_WIDTH 9

#define BUFFER_ROW 3
#define BUFFER_WIDTH 20
#else // !DISPLAY_LARGE
#define ROWS     2
#define COLUMNS 16

#define KEYING_MODE_ROW 0
#define KEYING_MODE_COL 0
#define KEYING_MODE_WIDTH 5

#define SPEED_ROW 0
#define SPEED_COL 5
#define SPEED_WIDTH 2

#define XMITTER_ROW 0
#define XMITTER_COL 8
#define XMITTER_WIDTH 3

#define SIDETONE_ROW 0
#define SIDETONE_COL 11
#define SIDETONE_WIDTH 1

#define PROG_MODE_ROW 0
#define PROG_MODE_COL 12
#define PROG_MODE_WIDTH 1

#define INPUT_ROW 0
#define INPUT_COL 13
#define INPUT_WIDTH 4

#define BUFFER_ROW 1
#define BUFFER_WIDTH 16
#endif // !DISPLAY_LARGE

#if defined(DISPLAY_LARGE)
String xmit_strings[] = {
    "Practice ",
    "Xmitter 1",
    "Xmitter 2",
    "Xmitter 3",
    "Xmitter 4",
    "Recording"
};
#else // !defined(DISPLAY_LARGE)
String xmit_strings[] = {
    "CPO",
    "Tx1",
    "Tx2",
    "Tx3",
    "Tx4",
    "Mem"
};
#endif // !defined(DISPLAY_LARGE)

#if defined(DISPLAY_LARGE)
String key_modestrings[] = {
    "Iambic-A",
    "Iambic-B",
    "Ultimatc"
    "Semiauto",
    "Straight"
};
#else // !defined(DISPLAY_LARGE)
String key_modestrings[] = {
    "IamA",
    "IamB",
    "Ulti"
    "Bug ",
    "Strt"
    };
#endif // !defined(DISPLAY_LARGE)

#if defined(DISPLAY_LARGE)
static String input_strings[] = {
    "Norm Paddle",
    "Rev Paddle ",
    "Keyboard   ",
    "Memory     "
};
#else // !defined(DISPLAY_LARGE)
static String input_strings[] = {
    "Nor",
    "Rev",
    "Kbd",
    "Mem"
};
#endif // !defined(DISPLAY_LARGE)

static String prog_mode_string = "Prog";


display *system_display_manager = NULL;

void display_manager_initialize(void) {
    system_display_manager = new display();
    system_display_manager->init();

    system_display_manager->input_source(CONFIG_MANAGER_PADDLES_MODE(), 0);
}

display::display(void) {
    m_display = new LiquidCrystal_I2C(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

    int i;
    for (i=0; i<20; ++i) {
	m_buffer[i] = ' ';
    }
 }

void display::write_string_and_fill(uint8_t column, uint8_t row, const String &s, uint8_t width) {
    m_display->setCursor(column, row);
    if (s.length() > width) {
	m_display->write((const uint8_t*) s.c_str(), width);
    }
    else {
	m_display->write((const uint8_t*) s.c_str(), s.length());
	byte i;
	for (i=s.length(); i<width; ++i) {
	    m_display->write((const uint8_t*) " ", 1);
	}
    }
}

void display::init(void) {
    m_display->begin(COLUMNS, ROWS, LCD_5x8DOTS);
    m_display->on();
    m_display->clear();
    m_display->noCursor();
    m_display->noAutoscroll();
}

void display::key_mode(keyer_mode_t mode) {
    if ((KEYER_IAMBIC_A <= mode) && (KEYER_STRAIGHT >= mode)) {
	write_string_and_fill(KEYING_MODE_COL, KEYING_MODE_ROW, key_modestrings[mode], KEYING_MODE_WIDTH);
    }
}

void display::prog_mode(bool is_in_prog_mode) {
    if (is_in_prog_mode) {
	write_string_and_fill(PROG_MODE_COL, PROG_MODE_ROW, prog_mode_string, PROG_MODE_WIDTH);
    }
    else {
	write_string_and_fill(PROG_MODE_COL, PROG_MODE_ROW, "", PROG_MODE_WIDTH);
    }
}


void display::wpm(int wpm) {
    char buffer[10];

    m_display->setCursor(SPEED_COL, SPEED_ROW);
    sprintf(buffer, "%2dwpm", wpm);
    m_display->write((const uint8_t*) buffer, SPEED_WIDTH);
}


void display::xmit_mode(int xmitter) {
    // TODO:  Make the "5" depend on whether or not the memory feature is enabled
    if ((0 <= xmitter) && (5 >= xmitter)) {
	write_string_and_fill(XMITTER_COL, XMITTER_ROW, xmit_strings[xmitter], XMITTER_WIDTH);
    }
}


void display::input_source(input_mode_t mode, uint8_t which) {
    if ((mode >= 0) && (mode <= MODE_MEMORY)) {
	write_string_and_fill(INPUT_COL, INPUT_ROW, input_strings[mode], INPUT_WIDTH);
    }
    if (mode == MODE_MEMORY) {
	if (INPUT_WIDTH > 4) {
	    char buffer[3];
	    snprintf(buffer, 3, "%2d", which);
	    m_display->setCursor(INPUT_COL+7, INPUT_ROW);
	    m_display->write((const uint8_t *)buffer, 2);
	}
    }
}

void display::number(int number) {
#if defined(DISPLAY_LARGE)
    char buffer[NUMBER_WIDTH+1];

    snprintf(buffer, NUMBER_WIDTH+1, "%9d", number);
    m_display->setCursor(NUMBER_COL, NUMBER_ROW);
    m_display->write((const uint8_t *)buffer, NUMBER_WIDTH);
#endif // defined(DISPLAY_LARGE)
}

void display::clear_number() {
#if defined(DISPLAY_LARGE)
    char buffer[NUMBER_WIDTH+1];

    snprintf(buffer, NUMBER_WIDTH+1, "%9s", "");
    m_display->setCursor(NUMBER_COL, NUMBER_ROW);
    m_display->write((const uint8_t *)buffer, NUMBER_WIDTH);
#endif // defined(DISPLAY_LARGE)
}

void display::scrolling_text(char c) {
    int i;
    for (i=0; i<(BUFFER_WIDTH-1); ++i) {
	m_buffer[i] = m_buffer[i+1];
    }
    m_buffer[BUFFER_WIDTH-1] = c;
    m_display->setCursor(0, BUFFER_ROW);
    m_display->write(m_buffer, BUFFER_WIDTH);
}


void display::sidetone(int freq) {
    char buffer[10];

    m_display->setCursor(SIDETONE_COL, SIDETONE_ROW);
#if defined(DISPLAY_LARGE)
    if (0 != freq) {
	sprintf(buffer, "Tone %4d", freq);
    }
    else {
	sprintf(buffer, "Tone  Off");
    }
#else // !defined(DISPLAY_LARGE)
    if (0 != freq) {
	buffer[0] = 'S';
    }
    else {
	buffer[0] = ' ';
    }
#endif // !defined(DISPLAY_LARGE)
    m_display->write((const uint8_t*) buffer, SIDETONE_WIDTH);
}

#endif // LCD_DISPLAY
