#include <Arduino.h>

#include <LiquidCrystal_I2C.h>

#define LEFT_PADDLE 5
#define RIGHT_PADDLE 2

#define SIDETONE 4

#define PTT_1    13
#define KEY_OUT_1 11

#define SIDETONE_FREQUENCY 800

#define MIN_WPM 5L
#define MAX_WPM 40L
int potPin = A0;
int potValue = 0;
char buffer[100];

unsigned dot_twitches;
unsigned dash_twitches;
unsigned word_twitches;
unsigned long last_ms;
unsigned long next_state_transition_ms;

typedef enum keyer_state { KEY_DIT, KEY_DAH, KEY_UP } keyer_state_t;

keyer_state_t keyer_state;
keyer_state_t next_keyer_state;

LiquidCrystal_I2C display(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

unsigned find_wpm() {
    unsigned wpm;
    // The WPM is MIN_WPM + (MAX_WPM - MIN_WPM) * 1024 / potValue;
    potValue = analogRead(potPin);
    wpm = MIN_WPM + ((MAX_WPM - MIN_WPM) * potValue + 512L) / 1024L;
    display.home();
    sprintf(buffer, "%2d", wpm);
    display.write((const uint8_t*) buffer, strlen(buffer));
    return wpm;
}

void setup () {
    unsigned wpm;
    pinMode(LEFT_PADDLE, INPUT);
    digitalWrite(LEFT_PADDLE, HIGH);
    pinMode(RIGHT_PADDLE, INPUT);
    digitalWrite(RIGHT_PADDLE, HIGH);

    pinMode(SIDETONE, OUTPUT);
    digitalWrite(SIDETONE, LOW);

    last_ms = 100 + millis();
    next_state_transition_ms = last_ms;
    keyer_state = KEY_UP;
    next_keyer_state = KEY_UP;

    display.begin(20, 4, LCD_5x8DOTS);
    display.on();
    display.clear();
    display.noCursor();
}

void loop() {
    unsigned long now = millis();
    unsigned wpm;

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

    wpm = find_wpm();
    dot_twitches = 1000/wpm;
    dash_twitches = 3000/wpm;
    word_twitches = 7000/wpm;
}	
