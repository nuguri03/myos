/* ISR = Interrupt Service Routine */

#include "isr.h"
#include "pic.h"
#include "printf.h"

static irq_handler_t irq_handlers[16];

const char *interrupt_messages[] = {
    // 0 ~ 31: CPU Exceptions
    "Division By Zero",              // 0  #DE
    "Debug",                         // 1  #DB
    "Non Maskable Interrupt",        // 2  NMI
    "Breakpoint",                    // 3  #BP
    "Overflow",                      // 4  #OF
    "Bound Range Exceeded",          // 5  #BR
    "Invalid Opcode",                // 6  #UD
    "Device Not Available",          // 7  #NM
    "Double Fault",                  // 8  #DF
    "Coprocessor Segment Overrun",   // 9  (구형)
    "Invalid TSS",                   // 10 #TS
    "Segment Not Present",           // 11 #NP
    "Stack Fault",                   // 12 #SS
    "General Protection Fault",      // 13 #GP
    "Page Fault",                    // 14 #PF
    "Reserved",                      // 15
    "x87 Floating-Point Exception",  // 16 #MF
    "Alignment Check",               // 17 #AC
    "Machine Check",                 // 18 #MC
    "SIMD Floating-Point Exception", // 19 #XM
    "Virtualization Exception",      // 20 #VE
    "Reserved",                      // 21
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved",          // 22 ~ 31

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

void register_irq_handler(u8 irq, irq_handler_t handler) {
    if (irq >= (sizeof(irq_handlers) / sizeof(irq_handlers[0]))) {
        return;
    }
    irq_handlers[irq] = handler;
}

void isr_handler(struct registers *regs) {
    // CPU 예외
    if (regs->int_no < 32) {
        kprintf("\n[EXCEPTIONS] %s (Error Code: %u)\n", 
            interrupt_messages[regs->int_no],
            regs->error_code);

        kprintf("System Halt");
        while(1);
    }

    // 하드웨어 IRQ(Interrupt ReQuest)
    if (regs->int_no >= 32 && regs->int_no <= 47) {
        u8 irq = regs->int_no - 32;

        // irq_handlers에 등록되어 있다면 호출
        if (irq_handlers[irq]) {
            irq_handlers[irq](regs);
        }

        pic_send_eoi(regs->int_no);
    }
}