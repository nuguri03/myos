#include "serial.h"
#include "io.h"
/* COM1 포트 기준
 *
 * UART는 하나의 베이스 포트(0x3F8)에서
 * 오프셋으로 내부 레지스터를 구분함.
 */
#define SERIAL_PORT_BASE        0x3F8

/* 베이스 포트 오프셋 레지스터 목록
 *
 * DLAB(Divisor Latch Access Bit) = 0일 때:
 *   BASE+0: THR (Transmit Holding Register) — 송신할 데이터 쓰기
 *   BASE+1: IER (Interrupt Enable Register) — 인터럽트 활성화 설정
 *
 * DLAB = 1일 때 (보레이트 설정 모드):
 *   BASE+0: Divisor Latch Low  — 보레이트 분주값 하위 8비트
 *   BASE+1: Divisor Latch High — 보레이트 분주값 상위 8비트
 *
 * DLAB 무관:
 *   BASE+2: FCR (FIFO Control Register)
 *   BASE+3: LCR (Line Control Register) — DLAB 비트도 여기 있음
 *   BASE+4: MCR (Modem Control Register)
 *   BASE+5: LSR (Line Status Register)  — 송신 준비 상태 확인
 */
#define SERIAL_DATA             (SERIAL_PORT_BASE + 0)
#define SERIAL_INT_ENABLE       (SERIAL_PORT_BASE + 1)
#define SERIAL_FIFO_CTRL        (SERIAL_PORT_BASE + 2)
#define SERIAL_LINE_CTRL        (SERIAL_PORT_BASE + 3)
#define SERIAL_MODEM_CTRL       (SERIAL_PORT_BASE + 4)
#define SERIAL_LINE_STATUS      (SERIAL_PORT_BASE + 5)

/* UART 보레이트 설정
 *
 * UART 클럭 = 115200 Hz (기준 주파수)
 * 분주값(Divisor) = 115200 / 원하는_보레이트
 *
 * 115200bps → Divisor = 1
 *  57600bps → Divisor = 2
 *   9600bps → Divisor = 12
 *
 * 분주값을 DLAB 모드에서 BASE+0(하위), BASE+1(상위)에 씀.
 */
#define SERIAL_BAUD_DIVISOR 1 /* 115200bps */

/* LCR (Line Control Register) 설정값
 *
 * 비트 0-1: 데이터 비트 수  (11 = 8비트)
 * 비트 2  : 스톱 비트 수    (0  = 1비트)
 * 비트 3-4: 패리티          (00 = 없음)
 * 비트 7  : DLAB            (1  = 보레이트 설정 모드)
 *
 * 0x03 = 0000 0011 → 8비트, 스톱 1비트, 패리티 없음 (8N1)
 * 0x80 = 1000 0000 → DLAB 활성화
 */
#define SERIAL_LCR_8N1 0x03
#define SERIAL_LCR_DLAB 0x80

/* FCR (FIFO Control Register) 설정값
 *
 * 비트 0: FIFO 활성화
 * 비트 1: 수신 FIFO 초기화
 * 비트 2: 송신 FIFO 초기화
 * 비트 6-7: 인터럽트 트리거 레벨 (11 = 14바이트)
 *
 * 0xC7 = 1100 0111
 */
#define SERIAL_FCR_ENABLE 0xC7

/* MCR (Modem Control Register) 설정값
 *
 * 비트 0: DTR (Data Terminal Ready)
 * 비트 1: RTS (Request To Send)
 * 비트 3: IRQ 활성화
 *
 * 0x0B = 0000 1011
 */
#define SERIAL_MCR_ENABLE 0x0B

/* LSR (Line Status Register) 비트 5
 *
 * THRE (Transmit Holding Register Empty)
 * 이 비트가 1이면 THR이 비어있어 다음 바이트를 송신할 수 있음.
 * 0이면 아직 송신 중 → 폴링으로 대기.
 */
#define SERIAL_LSR_THRE 0x20

void init_serial() {
    outb(SERIAL_INT_ENABLE, 0x00);  // 인터럽트 비활성화

    outb(SERIAL_LINE_CTRL, SERIAL_LCR_DLAB);    // DLAB 활성화
    

    // 분주값(?) 쓰기
    outb(SERIAL_DATA, (u8)(SERIAL_BAUD_DIVISOR & 0xFF));
    outb(SERIAL_INT_ENABLE, (u8)(SERIAL_BAUD_DIVISOR >> 8));

    outb(SERIAL_LINE_CTRL, SERIAL_LCR_8N1);    // DLAB 비활성화

    outb(SERIAL_FIFO_CTRL, SERIAL_FCR_ENABLE);  // FIFO 활성화 및 초기화

    outb(SERIAL_MODEM_CTRL, SERIAL_MCR_ENABLE); // DTR, RTS, IRQ 활성화
}

static void serial_wait_ready() {
    while (!(inb(SERIAL_LINE_STATUS & SERIAL_LSR_THRE)));
}

/* THR이 비워질 때까지 폴링으로 대기
 *
 * UART는 송신 속도가 CPU보다 훨씬 느림 (115200bps = 약 11KB/s).
 * THR에 데이터를 쓰기 전에 이전 바이트 송신이 끝났는지 확인해야 함.
 * LSR 비트 5(THRE)가 1이 될 때까지 반복.
 */
void serial_putchar(char c) {
    serial_wait_ready();
    outb(SERIAL_DATA, c);
}

void serial_print(const char *str) {
    while (*str) {
        // 터미널은 개행을 "\r\n"을 기대함
        if (*str == '\n') {
            serial_putchar('\r');
        }
        serial_putchar(*str);
        str++;
    }
}