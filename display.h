#if !defined(DISPLAY_H)
#define DISPLAY_H

#include <LiquidCrystal_I2C.h>

class display {
public:
    display(void);
    void init(void);
    void key_mode(const String &mode);
    void wpm(int wpm);
    void xmit_mode(const String &mode);
    void input_source(const String &mode);
    void scrolling_text(char c);
    void sidetone(int freq);

private:
    LiquidCrystal_I2C *m_display;
    void write_string_and_fill(uint8_t column, uint8_t row, const String &s, uint8_t width);
    uint8_t m_buffer[20];
};

#endif // !defined(DISPLAY_H)
