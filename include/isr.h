#ifndef ISR_H
#define ISR_H

#include "types.h"

struct registers {
    u32 ds;

    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;

    u32 int_no, error_code;

    u32 eip, cs, eflags, useresp, ss;
};

typedef void (*irq_handler_t)(struct registers*);

void isr_handler(struct registers* regs);
void register_irq_handler(u8 irq, irq_handler_t handler);

#endif