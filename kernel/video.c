#include "video.h"

static u16* video_memory = (u16*)0xB8000;
static position cursor = {0 , 0};

void clear_screen() {
    for (i32 i = 0; i < 80 * 25; i++) {
        video_memory[i] = (DEFAULT_COLOR << 8) | (u8)' ';
    }
    cursor.x = 0; cursor.y = 0;
}

static inline u32 get_offset(position pos) {
    return pos.y * WIDTH + pos.x;
}

i32 print_char(u8 ch) {
    if (cursor.x >= WIDTH) {
        cursor.x = 0;
        cursor.y++;
    }
    
    if (ch == '\n') {
        cursor.x = 0;
        cursor.y++;
    }
    else {
        u32 offset = get_offset(cursor);
        video_memory[offset] = (DEFAULT_COLOR << 8) | (u8)ch;
        cursor.x++;
    }

    return 1;
}

i32 print_string(u8* buf, i32 len) {
    if (len <= 0) {
        return -1;
    }
    
    i32 i;
    for (i = 0; i < len; i++) {
        print_char(buf[i]);
    }

    return i;
}