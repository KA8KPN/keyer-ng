#ifndef SERIAL_DEBUG_H_INCLUDED
#define SERIAL_DEBUG_H_INCLUDED

#include "options.h"

#ifdef FEATURE_SERIAL_DEBUG

void serial_debug_initialize(void);

int serial_log(const char *format, ...);

#define SERIAL_DEBUG_INITIALIZE() serial_debug_initialize()
#else // ! FEATURE_SERIAL_DEBUG
#define SERIAL_DEBUG_INITIALIZE()
#endif // ! FEATURE_SERIAL_DEBUG

#endif // SERIAL_DEBUG_H_INCLUDED
