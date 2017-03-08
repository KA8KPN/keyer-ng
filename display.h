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
    void wpm(int wpm, char c);
    void xmit_mode(int xmitter);
    void input_source(input_mode_t mode, uint8_t which);
    void scrolling_text(char c);
    void sidetone(int freq);
    void prog_mode(bool is_in_prog_mode);
    void number(int n);
    void clear_number(void);

private:
    LiquidCrystal_I2C *m_display;
    void write_string_and_fill(uint8_t column, uint8_t row, const String &s, uint8_t width);
    uint8_t m_buffer[20];
};

extern display *system_display_manager;
void display_manager_initialize(void);
#define DISPLAY_MANAGER_INITIALIZE()            display_manager_initialize()
#define DISPLAY_MANAGER_KEY_MODE(mode)          system_display_manager->key_mode(mode)
#define DISPLAY_MANAGER_WPM(wpm, c)             system_display_manager->wpm(wpm, c)
#define DISPLAY_MANAGER_XMIT_MODE(xmitter)      system_display_manager->xmit_mode(xmitter)
#define DISPLAY_MANAGER_INPUT_SOURCE(source, w) system_display_manager->input_source(source, w)
#define DISPLAY_MANAGER_SCROLLING_TEXT(c)       system_display_manager->scrolling_text(c)
#define DISPLAY_MANAGER_SIDETONE(freq)          system_display_manager->sidetone(freq)
#define DISPLAY_MANAGER_PROG_MODE(b)            system_display_manager->prog_mode(b)
#define DISPLAY_MANAGER_NUMBER(n)               system_display_manager->number(n)
#define DISPLAY_MANAGER_CLEAR_NUMBER()          system_display_manager->clear_number()
#else // !LCD_DISPLAY
#define DISPLAY_MANAGER_INITIALIZE()
#define DISPLAY_MANAGER_KEY_MODE(mode)          ((void)mode)
#define DISPLAY_MANAGER_WPM(wpm, c)             ((void)wpm, c)
#define DISPLAY_MANAGER_XMIT_MODE(xmitter)      ((void)xmitter)
#define DISPLAY_MANAGER_INPUT_SOURCE(source, w) ((void)source,w)
#define DISPLAY_MANAGER_SCROLLING_TEXT(c)       ((void)c)
#define DISPLAY_MANAGER_SIDETONE(freq)          ((void)freq)
#define DISPLAY_MANAGER_PROG_MODE(b)            ((void)b)
#define DISPLAY_MANAGER_NUMBER(n)               ((void)n)
#define DISPLAY_MANAGER_CLEAR_NUMBER()

#endif // LCD_DISPLAY

#endif // !defined(DISPLAY_H)
