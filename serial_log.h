#ifndef SERIAL_LOG_H_INCLUDED
#define SERIAL_LOG_H_INCLUDED

#include "options.h"

#ifdef FEATURE_SERIAL_LOG

void serial_log_initialize(void);

int serial_log(const char *format, ...);

#define SERIAL_LOG_INITIALIZE() serial_log_initialize()
#else // ! FEATURE_SERIAL_LOG
#define SERIAL_LOG_INITIALIZE()
#endif // ! FEATURE_SERIAL_LOG

#endif // SERIAL_LOG_H_INCLUDED
