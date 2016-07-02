#if !defined(DISPLAY_H)
#define DISPLAY_H

#include "options.h"

#ifdef LCD_DISPLAY
#include <LiquidCrystal_I2C.h>

#include "keyer.h"

class display {
public:
    display(void);
    void init(void);
    void key_mode(keyer_mode_t mode);
    void wpm(int wpm);
    void xmit_mode(int xmitter);
    void input_source(input_mode_t mode);
    void scrolling_text(char c);
    void sidetone(int freq);

private:
    LiquidCrystal_I2C *m_display;
    void write_string_and_fill(uint8_t column, uint8_t row, const String &s, uint8_t width);
    uint8_t m_buffer[20];
};

extern display *system_display_manager;
void display_manager_initialize(void);
#define DISPLAY_MANAGER_INITIALIZE()         display_manager_initialize()
#define DISPLAY_MANAGER_KEY_MODE(mode)       system_display_manager->key_mode(mode)
#define DISPLAY_MANAGER_WPM(wpm)             system_display_manager->wpm(wpm)
#define DISPLAY_MANAGER_XMIT_MODE(xmitter)   system_display_manager->xmit_mode(xmitter)
#define DISPLAY_MANAGER_INPUT_SOURCE(source) system_display_manager->input_source(source)
#define DISPLAY_MANAGER_SCROLLING_TEXT(c)    system_display_manager->scrolling_text(c)
#define DISPLAY_MANAGER_SIDETONE(freq)       system_display_manager->sidetone(freq)
#else // !LCD_DISPLAY
#define DISPLAY_MANAGER_INITIALIZE()
#define DISPLAY_MANAGER_KEY_MODE(mode)       ((void)mode)
#define DISPLAY_MANAGER_WPM(wpm)             ((void)wpm)
#define DISPLAY_MANAGER_XMIT_MODE(xmitter)   ((void)xmitter)
#define DISPLAY_MANAGER_INPUT_SOURCE(source) ((void)source)
#define DISPLAY_MANAGER_SCROLLING_TEXT(c)    ((void)c)
#define DISPLAY_MANAGER_SIDETONE(freq)       ((void)freq)

#endif // LCD_DISPLAY

#endif // !defined(DISPLAY_H)
