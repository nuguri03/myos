#ifndef ISR_H
#define ISR_H

#include "types.h"

struct registers {
    u32 ds;

    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;

    u32 int_no, error_code;

    u32 eip, cs, eflags, useresp, ss;
};

typedef struct registers registers_t;   

void isr_handler(registers_t regs);

#endif