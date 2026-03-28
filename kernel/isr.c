#include "isr.h"
#include "printf.h"

char *exception_messages[] = {
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
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_handler(registers_t regs) {
    kprintf("Received Interrupt: %d\n", regs.int_no);
    
    if (regs.int_no < 32) {
        kprintf("%s\n%x\n", exception_messages[regs.int_no], regs.error_code);

        kprintf("System Halt");

        while(1);
    }
}