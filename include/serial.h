#ifndef SERIAL_H
#define SERIAL_H

#include "types.h"

void init_serial();
void serial_putchar(char c);
void serial_print(const char *str);

#endif