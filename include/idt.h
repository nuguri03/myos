#ifndef IDT_H
#define IDT_H

#include "types.h"

struct idt_entry {
    u16 base_low;               // 핸들러 주소[15: 0]
    u16 sel;                    // GDT 코드 세그먼트 셀렉터
    u8 always0;                 // 항상 0
    u8 flags;                   // P, DPL, 타입
    u16 base_high;              // 핸들러 주소 [31:16]
} __attribute__((packed));

struct idt_ptr {
    u16 limit;
    u32 base;
} __attribute__((packed));

void init_idt();

#endif