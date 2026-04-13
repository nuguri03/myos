/* PIC = 여러 장치와 CPU 사이의 교통 정리 담당 */

#include "pic.h"
#include "io.h"

/* x86 I/O 포트 주소는 고정되어 있음 */

// PIC1 (Master)
#define PIC1_COMMAND 0x20   // ICW1, EOI 같은 제어 명령 전송
#define PIC1_DATA 0x21      // ICW2~4, 마스크 설정 전송

// PIC2 (Slave)
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

// ICW = Initialization Command Word
#define ICW1_ICW4 0x01      // ICW1 전송 시 ICW4도 전송할 것임
#define ICW1_INIT 0x11      // ICW1 초기화 명령 (비트4=1: ICW1식별, 비트0=1: ICW4 전송 예정)
#define ICW4_8086 0x01      // ICW4 8086 모드 설정 (비트0=1: 8086모드, 비트0=0: MCS-80/85 구형모드)


/* PIC 내부는 상태머신(현재 상태에 따라 다음 입력을 다르게 해석)으로 설계되어 있음으로,
    ICW는 무조건 순서대로 동작해야 함 */
void pic_remap(u8 offset1, u8 offset2) {
    // ICW1 = 초기화 시작
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    // ICW2 = 벡터 오프셋 설정
    outb(PIC1_DATA, offset1);   // Master = IRQ0 -> INT 0x20
    io_wait();
    outb(PIC2_DATA, offset2);   // Slave  = IRQ8 -> INT 0x28
    io_wait();

    // ICW3 = Master/Slave 설정
    outb(PIC1_DATA, 4);         // Master: IRQ2에 Slave 연결됨(4 = 0000 0100, 2번 비트)
    io_wait();
    outb(PIC2_DATA, 2);         // Slave: 자신이 IRQ2에 연결됨(2번에 연결됨)
    io_wait();
    // master는 slave가 여러개 연결될 수 있음으로 비트로 표현, slave는 아님으로 숫자로 표현

    // ICW4 = 동작 모드 설정, 8086모드로
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    // 마스크 해제 - 모든 IRQ 허용 ; 0x00 = 허용, 0xFF = 차단
    outb(PIC1_DATA, 0x00);
    outb(PIC2_DATA, 0x00);
}

// EOI = End Of Interrupt ; 인터럽트가 끝났으니 다음 IRQ를 받아도 된다는 신호
void pic_send_eoi(u8 int_no) {
    u8 irq = int_no - 32;

    // 0x20 = 0010 0000 ; Non-Specific EOI 명령
    // "현재 처리 중인 IRQ 완료" 를 PIC에 알리는 신호
    if (irq >= 8) {
        outb(PIC2_COMMAND, 0x20);
    }
    outb(PIC1_COMMAND, 0x20);
}