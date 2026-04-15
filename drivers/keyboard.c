#include "keyboard.h"
#include "io.h"
#include "isr.h"

/* 스캔코드 Set 1 → ASCII 변환 테이블 (Shift 없음)
 *
 * 인덱스 = 스캔코드 make 값
 * 값     = 대응하는 ASCII 문자 (매핑 없는 키는 0)
 *
 * IBM PC AT 표준 스캔코드 Set 1 기준.
 * BIOS가 키보드 컨트롤러(8042)를 초기화할 때 Set 1을 기본으로 설정함.
 */
static const char scancode_table[128] = {
/*00*/  0,    0,   '1', '2', '3', '4', '5', '6',
/*08*/ '7', '8', '9', '0', '-', '=',  '\b', '\t',
/*10*/ 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
/*18*/ 'o', 'p', '[', ']', '\n',  0,  'a', 's',
/*20*/ 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
/*28*/ '\'', '`',  0, '\\', 'z', 'x', 'c', 'v',
/*30*/ 'b', 'n', 'm', ',', '.', '/',  0,   '*',
/*38*/  0,  ' ',  0,   0,   0,   0,   0,   0,
};

/* 스캔코드 Set 1 → ASCII 변환 테이블 (Shift 있음) */
static const char scancode_table_shifted[128] = {
/*00*/  0,    0,   '!', '@', '#', '$', '%', '^',
/*08*/ '&', '*', '(', ')', '_', '+',  0,    0,
/*10*/ 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
/*18*/ 'O', 'P', '{', '}', '\n', 0,  'A', 'S',
/*20*/ 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
/*28*/ '"',  '~', 0,  '|', 'Z', 'X', 'C', 'V',
/*30*/ 'B', 'N', 'M', '<', '>', '?',  0,   '*',
/*38*/  0,  ' ',  0,   0,   0,   0,   0,   0,
};

/* 8042 PS/2 컨트롤러 데이터 포트
 * 키 이벤트 발생 시 스캔코드가 이 포트의 Output Buffer에 적재됨.
 * inb(0x60)으로 1바이트를 읽으면 Output Buffer가 비워지고
 * 8042가 다음 IRQ1을 발생시킬 준비가 됨.
 */
#define KEYBOARD_DATA_PORT      0x60

/* 스캔코드 비트 7 = break/make 구분 플래그
 * 비트 7이 1 → break code (키를 뗌): 0x80 ~ 0xFF
 * 비트 7이 0 → make  code (키를 누름): 0x00 ~ 0x7F
 *
 * break code = make code | 0x80
 * make  code = break code & ~0x80 (비트 7을 0으로 마스킹)
 */
#define KEYBOARD_BREAK_MASK     0x80

/* Shift 키의 make 스캔코드
 * Left Shift: 0x2A, Right Shift: 0x36
 * Shift는 단독으로 ASCII 값이 없는 modifier key이므로
 * 버퍼에 넣지 않고 shift_pressed 상태만 토글함.
 */
#define SCANCODE_LSHIFT         0x2A
#define SCANCODE_RSHIFT         0x36

/* 키보드 입력 원형 버퍼 (ring buffer)
 *
 * IRQ1 핸들러가 push, 커널이 getchar로 pop.
 * head == tail: 버퍼 비어있음
 * 유효 데이터 범위: [head, tail) — head 포함, tail 미포함
 */
#define KB_BUFFER_SIZE 256

static char kb_buffer[KB_BUFFER_SIZE];
static volatile u32 kb_buf_head = 0;  /* 커널이 다음에 읽을 위치 */
static volatile u32 kb_buf_tail = 0;  /* 핸들러가 다음에 쓸 위치   */

/* Shift 키 현재 상태 (눌려있으면 true) */
static volatile bool lshift_pressed = false;
static volatile bool rshift_pressed = false;

static bool is_shift_pressed() {
    return lshift_pressed || rshift_pressed;
}

/* 버퍼에 문자 1개를 씀 (IRQ1 핸들러에서 호출)
 *
 * 버퍼가 가득 차면 새 입력을 버림.
 * 오래된 입력을 버리려면 IRQ와 메인 루프가 동시에 head를 건드리게 되어
 * race condition이 발생하기 때문.
 */
static void kb_buffer_push(char c) {
    u32 next_tail = (kb_buf_tail + 1) % KB_BUFFER_SIZE;

    if (next_tail == kb_buf_head) return;

    kb_buffer[kb_buf_tail] = c;
    kb_buf_tail = next_tail;
}

/* 버퍼에서 문자 1개를 꺼냄 (커널에서 호출)
 *
 * 버퍼가 비어있으면 0 반환.
 * 반환 후 head를 전진시켜 해당 슬롯을 재사용 가능 상태로 만듦.
 */
char keyboard_getchar() {
    __asm__ volatile("cli");    // 인터럽트 비활성화
    if (kb_buf_head == kb_buf_tail) {
        __asm__ volatile("sti");    // 인터럽트 활성화
        return 0;
    }

    char c = kb_buffer[kb_buf_head];
    kb_buf_head = (kb_buf_head + 1) % KB_BUFFER_SIZE;
    __asm__ volatile("sti");
    return c;
}

/* IRQ1 인터럽트 핸들러 — 키 이벤트 처리
 *
 * IRQ1 발생 흐름:
 *   키 누름/뗌 → 8042가 스캔코드를 Output Buffer에 적재
 *   → IRQ1 발생 → PIC가 CPU에 INT 0x21 전달
 *   → IDT[0x21] → ISR 디스패처 → 이 함수 호출
 *
 * EOI는 ISR 디스패처가 핸들러 리턴 후 전송하므로 여기서는 보내지 않음.
 */
static void keyboard_handler(struct registers* regs) {
    /* 0x60 포트에서 스캔코드 읽기
     * 읽는 순간 8042의 Output Buffer가 비워져 다음 IRQ1을 받을 수 있게 됨 */
    u8 scancode = inb(KEYBOARD_DATA_PORT);

    /* break code 처리 (비트 7 = 1)
     * 키를 뗄 때만 발생. ASCII 변환 없이 Shift 상태만 업데이트하고 리턴. */
    if (scancode & KEYBOARD_BREAK_MASK) {
        u8 make = scancode & (~KEYBOARD_BREAK_MASK);  /* 비트 7을 0으로 → make code 복원 */
        if (make == SCANCODE_LSHIFT) { 
            lshift_pressed = false;
            return; 
        }
        if (make == SCANCODE_RSHIFT) { 
            rshift_pressed = false; 
            return; 
        }
        return;
    }

    /* Shift make code 처리
     * Shift 자체는 ASCII가 없으므로 버퍼에 넣지 않고 상태만 기록. */
    if (scancode == SCANCODE_LSHIFT) { 
        lshift_pressed = true; 
        return; 
    }
    if (scancode == SCANCODE_RSHIFT) { 
        rshift_pressed = true; 
        return;
    }

    /* 확장 스캔코드 방어 (0xE0 prefix 등)
     * 테이블 크기(128)를 벗어나는 스캔코드는 무시. */
    if (scancode >= 128) return;

    /* 스캔코드 → ASCII 변환
     * Shift 상태에 따라 테이블 선택. */
    char c = is_shift_pressed()
            ? scancode_table_shifted[scancode]
            : scancode_table[scancode];

    /* 매핑된 문자가 있을 때만 버퍼에 push (0 = 매핑 없음) */
    if (c != 0) {
        kb_buffer_push(c);
    }
}

/* 키보드 드라이버 초기화
 * IRQ1 핸들러를 ISR 디스패처에 등록. */
void init_keyboard() {
    register_irq_handler(1, keyboard_handler);
}