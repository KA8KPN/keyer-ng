#ifndef KEYER_H_INCLUDED
#define KEYER_H_INCLUDED

#include "options.h"

typedef enum keyer_mode { MODE_PADDLE_NORMAL, MODE_PADDLE_REVERSE, MODE_KEYBOARD, MODE_SERIAL, MODE_MEMORY } keyer_mode_t;

#define MIN_WPM 5L
#define MAX_WPM 40L
#define MS_PER_DOT 1200
#define MS_PER_DASH (3*MS_PER_DOT)
#define MS_PER_WORD (7*MS_PER_DOT)

#define LEFT_PADDLE 5
#define RIGHT_PADDLE 2

#define PTT_1    13
#define KEY_OUT_1 11
#define SIDETONE_FREQUENCY 800

extern String input_strings[];

#endif // KEYER_H_INCLUDED
