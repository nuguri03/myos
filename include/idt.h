#ifndef IDT_H
#define IDT_H

#include "types.h"

struct idt_entry {
    u16 base_low;
    u16 sel;     
    u8 always0;  
    u8 flags;    
    u16 base_high;
} __attribute__((packed));

struct idt_ptr {
    u16 limit;
    u32 base;
} __attribute__((packed));

void init_idt();

#endif