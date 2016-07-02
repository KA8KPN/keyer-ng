#include <Arduino.h>

#include "options.h"

#include "keying.h"
#include "display.h"
#include "wpm.h"
#include "paddles.h"
#include "serial.h"
#include "ps2_keyboard.h"
#include "morse_to_text.h"
#include "config_manager.h"

#include "keyer.h"

wpm wpm(A0);

input_mode_t input_mode;

#define DEFINE_MORSE_TABLES
#include "morse_tables.h"

void setup () {
    input_mode = MODE_PADDLE_NORMAL;

    DISPLAY_MANAGER_INITIALIZE();  // This needs to be first because the other options may write to it
    MORSE_TO_TEXT_INITIALIZE(&wpm);
    KEYING_INITIALIZE();
    PADDLES_INITIALIZE(&wpm, RIGHT_PADDLE, LEFT_PADDLE);
    SERIAL_INITIALIZE();
    PS2_KEYBOARD_INITIALIZE();
    CONFIG_MANAGER_INITIALIZE();  // This needs to be after all the configurable options
}

void loop() {
    unsigned long now = millis();

    input_mode = PADDLES_UPDATE(now, input_mode);
    SERIAL_UPDATE();
    PS2_KEYBOARD_UPDATE();
    input_mode = MORSE_TO_TEXT_UPDATE(now, input_mode);
    wpm.update();
}	
