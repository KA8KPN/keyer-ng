#include <Arduino.h>

#include "display.h"

display::display(void) {
    m_display = new LiquidCrystal_I2C(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
 }


void display::init(void) {
    m_display->begin(20, 4, LCD_5x8DOTS);
    m_display->on();
    m_display->clear();
    m_display->noCursor();
}

void display::key_mode(const String &mode) {
    m_display->home();
    m_display->write((const uint8_t*) mode.c_str(), 4);
}


void display::wpm(int wpm) {
    char buffer[10];

    m_display->setCursor(4, 0);
    sprintf(buffer, "%3d", wpm);
    m_display->write((const uint8_t*) buffer, 3);
}


void display::xmit_mode(const String &mode) {
    m_display->setCursor(8, 0);
    m_display->write((const uint8_t*) mode.c_str(), 4);
}


void display::paddle_orientation(const String &mode) {
    m_display->setCursor(13, 0);
    m_display->write((const uint8_t*) mode.c_str(), 3);
}


void display::input_source(const String &mode) {
    m_display->setCursor(17, 0);
    m_display->write((const uint8_t*) mode.c_str(), 3);
}


void display::scrolling_text(char c) {
}


void display::scrolling_text(char *s) {
}
