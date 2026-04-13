/* ISR = Interrupt Service Routine */

#include "isr.h"
#include "pic.h"
#include "printf.h"

const char *interrupt_messages[] = {
    // 0 ~ 31: CPU Exceptions
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved",

    // 32 ~ 47: Hardware Interrupts
    "IRQ0: System Timer",
    "IRQ1: Keyboard",
    "IRQ2: Cascade (Slave PIC)",
    "IRQ3: COM2 (Serial Port)",
    "IRQ4: COM1 (Serial Port)",
    "IRQ5: LPT2 (Parallel Port)",
    "IRQ6: Floppy Disk",
    "IRQ7: LPT1 (Parallel Port)",
    "IRQ8: Real Time Clock",
    "IRQ9: Redirected IRQ2",
    "IRQ10: Reserved",
    "IRQ11: Reserved",
    "IRQ12: PS/2 Mouse",
    "IRQ13: FPU / Coprocessor",
    "IRQ14: Primary ATA",
    "IRQ15: Secondary ATA"
};

void isr_handler(struct registers *regs) {
    kprintf("Received Interrupt: %d\n", (i32)regs->int_no);
    
    // CPU 예외
    if (regs->int_no < (u32)32) {
        kprintf("\n[EXCEPTIONS] %s (Error Code: %d)\n", interrupt_messages[regs->int_no], (i32)regs->error_code);

        kprintf("System Halt");
        while(1);
    }

    // 하드웨어 IRQ(Interrupt ReQuest)
    if (regs->int_no >= (u32)32 && regs->int_no <= (u32)47) {

        // TODO: 실제 하드웨어 인터럽트 함수 만들기

        pic_send_eoi(regs->int_no);
    }
}