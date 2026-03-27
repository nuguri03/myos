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
static inline u32 get_offset(position pos);
i32 print_char(u8 ch);
i32 print_string(u8* buf, i32 len);

#endif