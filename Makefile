BOARD_TAG    = uno
# The NewliquidCrystal library is available here:
# https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home
# The PS/2 Keyboard is the latest from http://playground.arduino.cc/Main/PS2Keyboard
ARDUINO_LIBS = NewliquidCrystal Wire PS2Keyboard
USER_LIB_PATH = ${PWD}/libraries
MONITOR_PORT = /dev/ttyACM0

include /usr/share/arduino/Arduino.mk

