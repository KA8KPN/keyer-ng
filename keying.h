#if !defined(KEYING_H)
#define KEYING_H

typedef enum keyer_state { KEY_DIT, KEY_DAH, KEY_UP } keyer_state_t;

class keying {
public:
    keying(int ptt_line, int key_out_line, int sidetone_freq);
    void key_up(void);
    void key_down(void);
    void ptt_pushed(void);
    void ptt_released(void);
    void set_sidetone_freq(int sidetone_freq);

private:
    void set_ptt(int ptt_line);
    void set_key_out(int key_out_line);
    byte m_ptt_line;
    byte m_key_out_line;
    int m_sidetone_freq;
    void (*m_key_action)(byte, byte);
    void (*m_ptt_action)(byte, byte);
    
};

#endif // !defined KEYING_H
