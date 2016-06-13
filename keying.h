#if !defined(KEYING_H)
#define KEYING_H

#define SIDETONE 4

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
    int m_ptt_line;
    int m_key_out_line;
    int m_sidetone_freq;
    void (*m_key_action)(int, bool);
    void (*m_sidetone_action)(int, bool);
    void (*m_ptt_action)(int, bool);
    
};

#endif // !defined KEYING_H
