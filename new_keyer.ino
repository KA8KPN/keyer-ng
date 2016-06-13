#include <Arduino.h>

#define LEFT_PADDLE 5
#define RIGHT_PADDLE 2

#define SIDETONE 4

#define PTT_1    13
#define KEY_OUT_1 11

#define SIDETONE_FREQUENCY 800

#define WPM 16

unsigned dot_twitches;
unsigned dash_twitches;
unsigned word_twitches;
unsigned long last_ms;
unsigned long next_state_transition_ms;

typedef enum keyer_state { KEY_DIT, KEY_DAH, KEY_UP } keyer_state_t;

keyer_state_t keyer_state;
keyer_state_t next_keyer_state;

void setup () {
     pinMode(LEFT_PADDLE, INPUT);
     digitalWrite(LEFT_PADDLE, HIGH);
     pinMode(RIGHT_PADDLE, INPUT);
     digitalWrite(RIGHT_PADDLE, HIGH);

     pinMode(SIDETONE, OUTPUT);
     digitalWrite(SIDETONE, LOW);

     dot_twitches = 1000/WPM;
     dash_twitches = 3000/WPM;
     word_twitches = 7000/WPM;
     last_ms = millis();
     next_state_transition_ms = last_ms;
     keyer_state = KEY_UP;
     next_keyer_state = KEY_UP;
}

void loop() {
    unsigned long now = millis();

    if (now >= next_state_transition_ms) {
	switch(keyer_state) {
	case KEY_DIT:
	    noTone(SIDETONE);
	    next_state_transition_ms = now + dot_twitches;
	    keyer_state = KEY_UP;
	    if (!digitalRead(RIGHT_PADDLE)) {
		next_keyer_state = KEY_DAH;
	    }
	    break;

	case KEY_DAH:
	    noTone(SIDETONE);
	    next_state_transition_ms = now + dot_twitches;
	    keyer_state = KEY_UP;
	    if (!digitalRead(LEFT_PADDLE)) {
		next_keyer_state = KEY_DIT;
	    }
	    break;

	case KEY_UP:
	    switch(next_keyer_state) {
	    case KEY_DIT:
		next_state_transition_ms = now + dot_twitches;
		tone(SIDETONE, SIDETONE_FREQUENCY);
		break;

	    case KEY_DAH:
		next_state_transition_ms = now + dash_twitches;
		tone(SIDETONE, SIDETONE_FREQUENCY);
		break;

	    default:
		if (!digitalRead(LEFT_PADDLE)) {
		    next_keyer_state = KEY_DIT;
		    next_state_transition_ms = now + dot_twitches;
		    tone(SIDETONE, SIDETONE_FREQUENCY);
		}
		else if (!digitalRead(RIGHT_PADDLE)) {
		    next_keyer_state = KEY_DAH;
		    next_state_transition_ms = now + dash_twitches;
		    tone(SIDETONE, SIDETONE_FREQUENCY);
		}
		break;
	    }
	    keyer_state = next_keyer_state;
	    next_keyer_state = KEY_UP;
	    break;
	}
    }
}	
