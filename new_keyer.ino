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

keying transmitter(PTT_1, KEY_OUT_1, SIDETONE_FREQUENCY);
wpm wpm(A0);
morse_to_text mtt(&transmitter, &wpm);
serial serial(&mtt, true);
ps2_keyboard keyboard(&mtt);

keyer_mode_t keyer_mode;

#define DEFINE_MORSE_TABLES
#include "morse_tables.h"

void setup () {
    pinMode(SIDETONE, OUTPUT);
    digitalWrite(SIDETONE, LOW);

    keyer_mode = MODE_PADDLE_NORMAL;

    // Without the serial support, the firmware takes 11316 bytes
    // Initial support, 12054 bytes
    // With space 12096
    // with working space 12164
    PADDLES_INITIALIZE(&transmitter, &wpm, RIGHT_PADDLE, LEFT_PADDLE);
    serial_setup();
    ps2_keyboard_setup();
    CONFIG_MANAGER_INITIALIZE();  // This needs to be last of the configurable options

    // The display isn't configurable.  It either exists, or it doesn't
    DISPLAY_MANAGER_INITIALIZE();
}

void loop() {
    unsigned long now = millis();

    keyer_mode = PADDLES_UPDATE(now, keyer_mode);
    serial.update();
    keyboard.update();
    keyer_mode = mtt.update(now, keyer_mode);
    wpm.update();
}	
