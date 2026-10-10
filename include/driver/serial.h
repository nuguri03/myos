#ifndef SERIAL_H
#define SERIAL_H

#include "types.h"

void init_serial();
ssize_t serial_printf(const char *fmt, ...);

#endif