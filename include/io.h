#ifndef IO_H
#define IO_H

#include "types.h"

void outb(u16 port, u8 data);
void inb(u16 port);

#endif