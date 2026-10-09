#include "serial.h"
#include "io.h"

#include "printf.h"

/* COM1: base port 0x3F8, offset으로 내부 레지스터 구분 */
#define SERIAL_PORT_BASE    0x3F8

#define SERIAL_DATA         (SERIAL_PORT_BASE + 0)  // THR: 송신 데이터 / Divisor Low  (DLAB=1)
#define SERIAL_INT_ENABLE   (SERIAL_PORT_BASE + 1)  // IER: 인터럽트 설정 / Divisor High (DLAB=1)
#define SERIAL_FIFO_CTRL    (SERIAL_PORT_BASE + 2)  // FCR: FIFO 제어
#define SERIAL_LINE_CTRL    (SERIAL_PORT_BASE + 3)  // LCR: 데이터 포맷 설정, DLAB bit 포함
#define SERIAL_MODEM_CTRL   (SERIAL_PORT_BASE + 4)  // MCR: DTR/RTS/IRQ 제어
#define SERIAL_LINE_STATUS  (SERIAL_PORT_BASE + 5)  // LSR: 송수신 상태 확인

/* baud rate divisor: 115200 / target_bps (115200bps = 1) */
#define SERIAL_BAUD_DIVISOR 1

#define SERIAL_LCR_8N1      0x03  // 0000 0011: 8bit, stop 1bit, no parity
#define SERIAL_LCR_DLAB     0x80  // 1000 0000: DLAB 활성화 → baud rate 설정 모드
#define SERIAL_FCR_ENABLE   0xC7  // 1100 0111: FIFO 활성화 + 수신/송신 초기화
#define SERIAL_MCR_ENABLE   0x0B  // 0000 1011: DTR, RTS, IRQ 활성화
#define SERIAL_LSR_THRE     0x20  // bit 5: THR empty → 송신 가능

void init_serial() {
    outb(SERIAL_INT_ENABLE, 0x00);                          // interrupt 비활성화

    outb(SERIAL_LINE_CTRL,  SERIAL_LCR_DLAB);               // DLAB 활성화 → baud rate 설정 모드
    
    outb(SERIAL_DATA,       (u8)(SERIAL_BAUD_DIVISOR & 0xFF)); // divisor low byte
    outb(SERIAL_INT_ENABLE, (u8)(SERIAL_BAUD_DIVISOR >> 8));   // divisor high byte
    
    outb(SERIAL_LINE_CTRL,  SERIAL_LCR_8N1);                // DLAB 비활성화 + 8N1 설정
    
    outb(SERIAL_FIFO_CTRL,  SERIAL_FCR_ENABLE);             // FIFO 활성화 및 초기화
    
    outb(SERIAL_MODEM_CTRL, SERIAL_MCR_ENABLE);             // DTR, RTS, IRQ 활성화
}

static void serial_wait_ready() {
    while (!(inb(SERIAL_LINE_STATUS) & SERIAL_LSR_THRE));
}

/* THR이 비워질 때까지 폴링으로 대기
 * LSR 비트 5(THRE)가 1이 될 때까지 반복하여 이전 바이트 송신이 끝났는지 확인 */
static void serial_putc(char c) {
    serial_wait_ready();
    outb(SERIAL_DATA, c);
}

ssize_t serial_kprintf(const char *fmt, ...) {
    char buf[1024];
    va_list args;

    va_start(args, fmt);

    ssize_t result = kvsnprintf(buf, sizeof(buf), fmt, args);

    va_end(args);

    if (result >= 0) {
        size_t stored = (size_t)result;

        // 출력이 짤렸을 때
        if (stored >= sizeof(buf)) {
            stored = sizeof(buf) - 1;
        }
        
        for (size_t i = 0; i < stored; i++) {
            if (buf[i] == '\n') {
                serial_putc('\r');
            }
            serial_putc(buf[i]);
        }
    }

    return result;
}