#include <Arduino.h>

#define LEFT_PADDLE 5
#define RIGHT_PADDLE 2

#define SIDETONE 4

#define SIDETONE_FREQUENCY 800

void setup () {
     pinMode(LEFT_PADDLE, INPUT);
     pinMode(RIGHT_PADDLE, INPUT);
     pinMode(SIDETONE, OUTPUT);
}

void loop() {
    while(1) {
	if (!(digitalRead(LEFT_PADDLE) || digitalRead(RIGHT_PADDLE))) {
	    tone(SIDETONE, SIDETONE_FREQUENCY);
	}
	else {
	    noTone(SIDETONE);
	}
    }
}	
