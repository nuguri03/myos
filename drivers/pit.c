/* PIT(Programmable Interval Timer)
   1193182Hz 클럭을 divisor로 나눠 원하는 주파수로 IRQ0를 발생시키는 칩 */

#include "pit.h"
#include "isr.h"
#include "io.h"

// PIT I/O 포트 주소 — x86 스펙에 고정
#define PIT_CHANNEL0    0x40    // Channel 0 데이터 포트 — IRQ0 발생용
#define PIT_COMMAND     0x43    // Mode/Command 레지스터

// PIT 입력 클럭 주파수 — 하드웨어 고정값
#define PIT_FREQUENCY   1193182

/* Command 바이트
   0x36 = 0011 0110
   비트 7~6 : 00  = Channel 0 선택
   비트 5~4 : 11  = LSB/MSB 순서로 전송
   비트 3~1 : 011 = Mode 3 (Square Wave)
   비트 0   : 0   = 16비트 바이너리 */
#define PIT_COMMAND_CHANNEL0 0x36

/* 부팅 후 경과 틱 수 — pit_handler 호출마다 1씩 증가 */
static volatile u32 pit_ticks = 0;

/* IRQ0 발생마다 호출되는 핸들러 */
static void pit_handler(struct registers* regs) {
    pit_ticks++;
}

/* 부팅 후 경과 틱 수 반환 */
u32 pit_get_ticks() {
    return pit_ticks;
}

void init_pit(u32 frequency) {
    /* PIT divisor는 16비트이므로 1~65535 범위만 표현 가능하다.
       따라서 frequency는 0이 될 수 없고, 너무 낮으면 divisor가 0xFFFF를 넘으므로
       표현 가능한 최소 주파수로 올려서(clamp) 사용한다. */
    if (frequency == 0) {
        frequency = 1;
    }

    u32 min_frequency = (PIT_FREQUENCY + 0xFFFF - 1) / 0xFFFF;
    if (frequency < min_frequency) {
        frequency = min_frequency;
    }

    if (frequency > PIT_FREQUENCY) {
        frequency = PIT_FREQUENCY;
    }

    /* divisor = PIT 입력 클럭 / 원하는 주파수
       frequency = 1000 → divisor = 1193  → 1ms마다 IRQ0 발생
       frequency = 100  → divisor = 11932 → 10ms마다 IRQ0 발생 */
    u32 divisor = PIT_FREQUENCY / frequency;

    // Channel 0을 Mode 3(Square Wave)으로 설정
    outb(PIT_COMMAND, PIT_COMMAND_CHANNEL0);

    // divisor를 LSB → MSB 순서로 Channel 0에 전송
    outb(PIT_CHANNEL0, (u8)(divisor & 0xFF));        
    outb(PIT_CHANNEL0, (u8)(divisor >> 8 & 0xFF));

    // IRQ0에 핸들러 등록 — 이제부터 주기적으로 pit_handler 호출됨 
    register_irq_handler(0, pit_handler);
}