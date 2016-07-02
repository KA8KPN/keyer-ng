#if !defined(KEYING_H)
#define KEYING_H

typedef enum keyer_state { KEY_DIT, KEY_DAH, KEY_UP } keyer_state_t;

class keying {
public:
    keying(int ptt_line, int key_out_line, int sidetone_freq);
    void key_up(void);
    void key_down(void);
    void ptt_push(void);
    void ptt_release(void);
    void set_sidetone_freq(int sidetone_freq);
    void toggle_sidetone_enable(void);

private:
    void set_ptt(int ptt_line);
    void set_key_out(int key_out_line);
    byte m_pttLine;
    byte m_keyOutLine;
    int m_sidetoneFreq;
    void (*m_keyAction)(byte, byte);
    void (*m_pttAction)(byte, byte);
    
};

void keying_initialize(void);
extern keying *system_transmitter;

#define KEYING_INITIALIZE()       keying_initialize()
#define TRANSMITTER_KEY_UP()      system_transmitter->key_up()
#define TRANSMITTER_KEY_DOWN()    system_transmitter->key_down()
#define TRANSMITTER_PTT_PUSH()    system_transmitter->ptt_push()
#define TRANSMITTER_PTT_RELEASE() system_transmitter->ptt_release()
#define SET_SIDETONE_FREQ(freq)   system_transmitter->set_sidetone_freq(freq)
#define TOGGLE_SIDETONE_ENABLE()  system_transmitter->toggle_sidetone_enable()

#endif // !defined KEYING_H
