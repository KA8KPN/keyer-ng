#if !defined(KEYING_H)
#define KEYING_H

typedef enum keyer_state { KEY_DIT, KEY_DAH, KEY_UP } keyer_state_t;

class keying {
public:
    keying(void);
    virtual void key_up(void) = 0;
    virtual void key_down(void) = 0;
    virtual void ptt_push(void) = 0;
    virtual void ptt_release(void) = 0;
    virtual void set_sidetone_freq(int sidetone_freq) = 0;
    virtual void toggle_sidetone_enable(void) = 0;
    virtual ~keying(void);
};


void keying_initialize(void);
void keying_config_mode(boolean enter_config_mode);
extern keying *system_transmitter;

#define KEYING_INITIALIZE()       keying_initialize()
#define TRANSMITTER_KEY_UP()      system_transmitter->key_up()
#define TRANSMITTER_KEY_DOWN()    system_transmitter->key_down()
#define TRANSMITTER_PTT_PUSH()    system_transmitter->ptt_push()
#define TRANSMITTER_PTT_RELEASE() system_transmitter->ptt_release()
#define SET_SIDETONE_FREQ(freq)   system_transmitter->set_sidetone_freq(freq)
#define TOGGLE_SIDETONE_ENABLE()  system_transmitter->toggle_sidetone_enable()
#define KEYING_PROGRAM_MODE(b)           keying_config_mode(b)

#endif // !defined KEYING_H
