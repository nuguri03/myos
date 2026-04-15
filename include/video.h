#ifndef VIDEO_H
#define VIDEO_H

#include "types.h"

#define WIDTH 80
#define HEIGHT 25
#define DEFAULT_COLOR 0x07

struct cursor_t {
    u16 x, y;
};

void clear_vga();

ssize_t vga_putchar(const char ch);
ssize_t vga_print_string(const char* buf, const ssize_t len);

#endif