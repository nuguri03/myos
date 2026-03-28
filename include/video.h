#ifndef VIDEO_H
#define VIDEO_H

#include "types.h"

#define WIDTH 80
#define HEIGHT 25
#define DEFAULT_COLOR 0x07

typedef struct {
    u16 x, y;
} cursor_t;

void clear_screen();
i32 vga_print_string(const char* buf, const i32 len);

#endif