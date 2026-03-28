#include "video.h"
#include "string.h"
#include "io.h"

static u16* video_memory = (u16*)0xB8000;
static position cursor = {0 , 0};

static void scroll() {
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

void clear_screen() {
    for (i32 i = 0; i < WIDTH * HEIGHT; i++) {
        video_memory[i] = (DEFAULT_COLOR << 8) | (u8)' ';
    }
    cursor.x = 0; cursor.y = 0;
    update_cursor();
}

static inline u32 get_offset(position pos) {
    return pos.y * WIDTH + pos.x;
}

static i32 itoa(const i32 num, char* str, u32 base) {
    i32 i = 0;
    u8 is_negative = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return i;
    }

    u32 unum;

    if (base == 10 && num < 0) {
        is_negative = 1;
        str[i++] = '-';
        unum = (u32)(-num);
    } else {
        unum = (u32)num;
    }

    while (unum != 0) {
        u32 rem = unum % base;

        str[i++] = (rem > 9) ? (rem - 10) + 'A' : rem + '0';
        unum = unum / base;
    }

    i32 start = is_negative ? 1 : 0;
    i32 end = i - 1;
    while (start < end) {
        u8 temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
    str[i] = '\0';

    return i;
}

static i32 print_char(const char ch) {
    if (ch == '\n') {
        cursor.x = 0;
        cursor.y++;
    } else if (ch == '\r') {
        cursor.x = 0;
    } else {
        u32 offset = get_offset(cursor);
        video_memory[offset] = (DEFAULT_COLOR << 8) | (u8)ch;
        cursor.x++;
    }

    if (cursor.x >= WIDTH) {
        cursor.x = 0;
        cursor.y++;
    }
    
    if (cursor.y >= HEIGHT) {
        scroll();
    }
    
    update_cursor();

    return 1;
}

i32 print_int(const i32 num) {
    char buf[32];

    itoa(num, buf, 10);
    print_string(buf);

    return 1;
}

i32 print_hex(const i32 num) {
    char buf[32];
    itoa(num, buf, 16);

    print_string("0x");
    print_string(buf);

    return 1;
}

i32 print_string(const char* buf) {
    i32 len = strlen((const char*)buf);
    if (len <= 0) {
        return -1;
    }
    
    i32 i;
    for (i = 0; i < len; i++) {
        print_char(buf[i]);
    }

    return i;
}