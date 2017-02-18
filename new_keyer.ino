#include <Arduino.h>

#include "options.h"

#include "keying.h"
#include "display.h"
#include "wpm.h"
#include "paddles.h"
#include "serial.h"
#include "ps2_keyboard.h"
#include "text_to_morse.h"
#include "buttons.h"
#include "config_manager.h"
#include "memories.h"

#include "keyer.h"

input_mode_t input_mode;

#define DEFINE_MORSE_TABLES
#include "morse_tables.h"

void setup () {
    input_mode = MODE_PADDLE_NORMAL;

    DISPLAY_MANAGER_INITIALIZE();  // This needs to be first because the other options may write to it
    WPM_INITIALIZE();
    TEXT_TO_MORSE_INITIALIZE();
    KEYING_INITIALIZE();
    PADDLES_INITIALIZE(RIGHT_PADDLE, LEFT_PADDLE);
    SERIAL_INITIALIZE();
    PS2_KEYBOARD_INITIALIZE();
    BUTTONS_INITIALIZE();
    MEMORIES_INITIALIZE();
    CONFIG_MANAGER_INITIALIZE();  // This needs to be after all the configurable options
}

void loop() {
    unsigned long now = millis();

    input_mode = PADDLES_UPDATE(now, input_mode);
    SERIAL_UPDATE();
    PS2_KEYBOARD_UPDATE();
    BUTTONS_UPDATE();
    input_mode = TEXT_TO_MORSE_UPDATE(now, input_mode);
    input_mode = MEMORIES_UPDATE(now, input_mode);
    WPM_UPDATE();
    KEYING_UPDATE(now);
}	
