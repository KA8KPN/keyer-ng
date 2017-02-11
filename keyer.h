#ifndef KEYER_H_INCLUDED
#define KEYER_H_INCLUDED

#include "options.h"

typedef enum input_mode { MODE_PADDLE_NORMAL, MODE_PADDLE_REVERSE, MODE_KEYBOARD, MODE_MEMORY } input_mode_t;

typedef enum keyer_mode {KEYER_IAMBIC_A, KEYER_IAMBIC_B, KEYER_ULTIMATIC, KEYER_SEMIAUTO, KEYER_STRAIGHT } keyer_mode_t;

#define ANALOG_IN_MAX 1023

#define MIN_WPM 5L
#define MAX_WPM 40L
#define DEFAULT_WPM 20
#define MS_PER_TWITCH 120
#define TWITCHES_PER_DOT 10
#define MS_PER_DOT    (TWITCHES_PER_DOT*MS_PER_TWITCH)
#define MS_PER_DASH   (3*MS_PER_DOT)
#define MS_PER_WORD   (7*MS_PER_DOT)
#define WPM_POT_PORT A0

#define BUTTONS_AI_PORT A1
#define NUM_BUTTONS 13

#define LEFT_PADDLE 5
#define RIGHT_PADDLE 2

#define PTT_1    13
#define KEY_OUT_1 11
#define PTT_DELAY_1 10
#define PTT_HANG_1 750
#define PTT_2    0
#define KEY_OUT_2 12
#define PTT_DELAY_2 0
#define PTT_HANG_2 0
#define PTT_3    0
#define KEY_OUT_3 0
#define PTT_DELAY_3 0
#define PTT_HANG_3 0
#define PTT_4    0
#define KEY_OUT_4 0
#define PTT_DELAY_4 0
#define PTT_HANG_4 0

#define SIDETONE 4
#define SIDETONE_FREQUENCY 800

#define KBD_DATA A3
#define KBD_IRQ 3

#endif // KEYER_H_INCLUDED
