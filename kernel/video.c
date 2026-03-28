#include "video.h"
#include "string.h"
#include "io.h"

static u16* video_memory = (u16*)0xB8000;
static struct cursor_t cursor = {0 , 0};

static void scroll_cursor() {
    for (i32 i = 0; i < WIDTH * (HEIGHT - 1); i++) {
        video_memory[i] = video_memory[i + WIDTH];
    }

    for (i32 i = WIDTH * (HEIGHT - 1); i < WIDTH * HEIGHT; i++) {
        video_memory[i] = (DEFAULT_COLOR << 8) | (u8)' ';
    }
    cursor.y = HEIGHT - 1;
}

static void update_cursor() {
    u16 pos = cursor.y * WIDTH + cursor.x;

    outb(0x3D4, 0x0E);
    outb(0x3D5, (u8)((pos >> 8) & 0xFF));

    outb(0x3D4, 0x0F);
    outb(0x3D5, (u8)(pos & 0xFF));
}

void clear_vga() {
    for (i32 i = 0; i < WIDTH * HEIGHT; i++) {
        video_memory[i] = (DEFAULT_COLOR << 8) | (u8)' ';
    }
    cursor.x = 0; cursor.y = 0;
    update_cursor();
}

static inline u32 get_offset(const u16 x, const u16 y) {
    return y * WIDTH + x;
}

static i32 vga_putc(const char ch) {
    if (ch == '\n') {
        cursor.x = 0;
        cursor.y++;
    } else if (ch == '\r') {
        cursor.x = 0;
    } else {
        u32 offset = get_offset(cursor.x, cursor.y);
        video_memory[offset] = (DEFAULT_COLOR << 8) | (u8)ch;
        cursor.x++;
    }

    if (cursor.x >= WIDTH) {
        cursor.x = 0;
        cursor.y++;
    }
    
    if (cursor.y >= HEIGHT) {
        scroll_cursor();
    }
    
    update_cursor();

    return 1;
}

i32 vga_print_string(const char* buf, const i32 len) {
    if (len <= 0) {
        return -1;
    }
    
    i32 i;
    for (i = 0; i < len; i++) {
        vga_putc(buf[i]);
    }

    return i;
}