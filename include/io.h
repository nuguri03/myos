#ifndef IO_H
#define IO_H

#include "types.h"

static inline void outb(u16 port, u8 data) {
    __asm__ __volatile__ ("outb %0, %1": : "a"(data), "dN"(port));
}

static inline u8 inb(u16 port) {
    u8 ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

static inline void io_wait() {
    outb(0x80, 0);
}

#endif