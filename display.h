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
    void paddle_orientation(const String &mode);
    void input_source(const String &mode);
    void scrolling_text(char c);
    void scrolling_text(char *s);

private:
    LiquidCrystal_I2C *m_display;
};

#endif // !defined(DISPLAY_H)
