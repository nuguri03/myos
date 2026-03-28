#ifndef VIDEO_H
#define VIDEO_H

#include "types.h"

#define WIDTH 80
#define HEIGHT 25
#define DEFAULT_COLOR 0x07

typedef struct {
    u16 x, y;
} position;

void clear_screen();
i32 print_string(const char* buf);
i32 print_int(const i32 num);
i32 print_hex(const i32 num);

#endif