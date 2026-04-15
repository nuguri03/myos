#include "video.h"
#include "io.h"

// 0xB8000 = VGA 텍스트 모드 프레임버퍼의 시작 주소
static u16* video_memory = (u16*)0xB8000;
static struct cursor_t cursor = {0 , 0};

/* 아래 칸에 있는 문자를 위로 옮기고, 밑으로 내려간 만큼 빈 공간을 검은색으로 채우는 함수 */
static void scroll_cursor() {
    // 아래 칸에 있는 문자를 위로 옮김
    for (i32 i = 0; i < WIDTH * (HEIGHT - 1); i++) {
        video_memory[i] = video_memory[i + WIDTH];
    }

    // 밑으로 내려간 만큼 빈 공간을 검은색으로 채움
    for (i32 i = WIDTH * (HEIGHT - 1); i < WIDTH * HEIGHT; i++) {
        // 상위 1바이트(8비트) = 색상, 하위 1바이트(8비트) = 문자
        video_memory[i] = (u16)((DEFAULT_COLOR << 8) | ' ');
    }

    // 스크롤 후 커서는 새로 생긴 빈 마지막 줄에 있어야 함.
    cursor.y = HEIGHT - 1;
}

static void update_cursor() {
    u16 pos = cursor.y * WIDTH + cursor.x;

    // 0x3D4 = 인덱스 포트(어떤 레지스터를 건드릴지 선택)
    // 0x3D5 = 데이터 포트(선택된 레지스터에 값 쓰기)
    // 0x0E = 하드웨어 커서 레지스터의 상위 8비트
    // 0x0F = 하드웨어 커서 레지스터의 하위 8비트

    outb(0x3D4, 0x0E);
    outb(0x3D5, (u8)((pos >> 8) & 0xFF));

    outb(0x3D4, 0x0F);
    outb(0x3D5, (u8)(pos & 0xFF));
}

/* 모든 칸을 검은색으로 채우는 함수 */
void clear_vga() {
    for (i32 i = 0; i < WIDTH * HEIGHT; i++) {
        video_memory[i] = (u16)((DEFAULT_COLOR << 8) | ' ');
    }
    cursor.x = 0; cursor.y = 0;
    update_cursor();
}

/* {x, y}의 주소를 받는 함수 */
static inline u32 get_offset(const u16 x, const u16 y) {
    return y * WIDTH + x;
}

/* 문자 하나를 vga에 print하고, cursor를 업데이트하는 함수 */
ssize_t vga_putchar(const char ch) {
    if (ch == '\n') {
        cursor.x = 0;
        cursor.y++;
    } 
    else if (ch == '\r') {
        cursor.x = 0;
    }
    else if (ch == '\b') {
        if (cursor.x > 0) {
            cursor.x--;
        } 
        else if (cursor.y > 0) {
            cursor.y--;
            cursor.x = WIDTH - 1;
            while (cursor.x > 0) {
                u32 offset = get_offset(cursor.x, cursor.y);
                char ch_at = (char)(video_memory[offset] & 0xFF);
                if (ch_at != ' ') break;
                cursor.x--;
            }
            cursor.x++;
        }
        u32 offset = get_offset(cursor.x, cursor.y);
        video_memory[offset] = (u16)((DEFAULT_COLOR << 8) | ' ');
    } 
    else if (ch == '\t') {
        u32 next_tab = (cursor.x + 8) & ~7;         // (cursor + 8) / 8 * 8 과 동일
        while (cursor.x < next_tab && cursor.x < WIDTH) {
            u32 offset = get_offset(cursor.x, cursor.y);
            video_memory[offset] = (u16)((DEFAULT_COLOR << 8) | ' ');
            cursor.x++;
        }
    }
    else {
        u32 offset = get_offset(cursor.x, cursor.y);
        video_memory[offset] = (u16)((DEFAULT_COLOR << 8) | ch);
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

/* vga_putchar를 활용하여 문자열을 출력하는 함수 */
ssize_t vga_print(const char* str, const ssize_t len) {
    if (str == NULL || len < 0) {
        return -1;
    }
    if (len == 0) {
        return 0;
    }
    
    ssize_t i;
    for (i = 0; i < len; i++) {
        vga_putchar(str[i]);
    }

    return i;
}