#include <Arduino.h>

#include "display.h"

#ifdef LCD_DISPLAY

#include "config_manager.h"

#if defined(DISPLAY_LARGE)
#define ROWS     4
#define COLUMNS 20

#define KEYING_MODE_ROW 0
#define KEYING_MODE_COL 0
#define KEYING_MODE_WIDTH 15

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
#define INPUT_WIDTH 20

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

#define INPUT_ROW 0
#define INPUT_COL 13
#define INPUT_WIDTH 4

#define BUFFER_ROW 1
#define BUFFER_WIDTH 16
#endif // !DISPLAY_LARGE

#if defined(DISPLAY_LARGE)
String xmit_strings[] = {
    "Xmitter 1",
    "Xmitter 2",
    "Xmitter 3",
    "Xmitter 4",
    "Practice "
};
#else // !defined(DISPLAY_LARGE)
String xmit_strings[] = {
    "Tx1",
    "Tx2",
    "Tx3",
    "Tx4",
    "CPO"
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
    "Serial Port",
    "Memory ##  "
};
#else // !defined(DISPLAY_LARGE)
static String input_strings[] = {
    "Nor",
    "Rev",
    "Kbd",
    "Srl",
    "Mem"
};
#endif // !defined(DISPLAY_LARGE)


display *system_display_manager = NULL;

void display_manager_initialize(void) {
    system_display_manager = new display();
    system_display_manager->init();

    system_display_manager->key_mode(key_modestrings[0]);
    system_display_manager->sidetone(SIDETONE_FREQUENCY);
    system_display_manager->input_source(CONFIG_MANAGER_PADDLES_MODE());
    system_display_manager->xmit_mode(xmit_strings[0]);
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

void display::key_mode(const String &mode) {
    write_string_and_fill(KEYING_MODE_COL, KEYING_MODE_ROW, mode, KEYING_MODE_WIDTH);
}


void display::wpm(int wpm) {
    char buffer[10];

    m_display->setCursor(SPEED_COL, SPEED_ROW);
    sprintf(buffer, "%2dwpm", wpm);
    m_display->write((const uint8_t*) buffer, SPEED_WIDTH);
}


void display::xmit_mode(const String &mode) {
    write_string_and_fill(XMITTER_COL, XMITTER_ROW, mode, XMITTER_WIDTH);
}


void display::input_source(keyer_mode_t mode) {
    if ((mode >= 0) && (mode <= MODE_MEMORY)) {
	write_string_and_fill(INPUT_COL, INPUT_ROW, input_strings[mode], INPUT_WIDTH);
    }
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
