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

#if defined(DISPLAY_LARGE)
String xmit_strings[] = {
    "Xmitter 1",
    "Xmitter 2",
    "Xmitter 3",
    "Xmitter 4",
    "Practice "
};
#else // !defined(DISPLAY_LARGE)
String key_modestrings[] = {
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
String input_strings[] = {
    "Norm Paddle",
    "Rev Paddle ",
    "Keyboard   ",
    "Serial Port",
    "Memory ##  "
};
#else // !defined(DISPLAY_LARGE)
String input_strings[] = {
    "Nor",
    "Rev",
    "Kbd",
    "Srl",
    "Mem"
};
#endif // !defined(DISPLAY_LARGE)

keying transmitter(PTT_1, KEY_OUT_1, SIDETONE_FREQUENCY);
display display_manager;
wpm wpm(A0, &display_manager);
morse_to_text mtt(&transmitter, &display_manager, &wpm);
serial serial(&display_manager, &mtt, true);
ps2_keyboard keyboard(&display_manager, &mtt);

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
    PADDLES_INITIALIZE(&transmitter, &display_manager, &wpm, RIGHT_PADDLE, LEFT_PADDLE);
    serial_setup();
    ps2_keyboard_setup();
    CONFIG_MANAGER_INITIALIZE();  // This needs to be last of the configurable options

    // The display isn't configurable.  It either exists, or it doesn't
    display_manager.init();
    
    display_manager.key_mode(key_modestrings[0]);
    display_manager.sidetone(SIDETONE_FREQUENCY);
    display_manager.input_source(input_strings[CONFIG_MANAGER_PADDLES_MODE()]);
    display_manager.xmit_mode(xmit_strings[0]);

}

void loop() {
    unsigned long now = millis();

    keyer_mode = PADDLES_UPDATE(now, keyer_mode);
    serial.update();
    keyboard.update();
    keyer_mode = mtt.update(now, keyer_mode);
    wpm.update();
}	
