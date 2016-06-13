BOARD_TAG    = uno
# The NewliquidCrystal library is available here:
# https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home
ARDUINO_LIBS = NewliquidCrystal Wire
USER_LIB_PATH = ${PWD}/libraries
MONITOR_PORT = /dev/ttyACM0

include /usr/share/arduino/Arduino.mk

