#ifndef GDT_H
#define GDT_H

#include "types.h"

/* Intel GDT 엔트리 구조체 — 8바이트 고정 */
struct gdt_entry {
    u16 limit_low;   // 바이트 0~1: limit[15:0]
    u16 base_low;    // 바이트 2~3: base[15:0]
    u8  base_middle; // 바이트 4  : base[23:16]
    u8  access;      // 바이트 5  : P, DPL, S, E, DC, RW, A
    u8  granularity; // 바이트 6  : G, D/B, L, AVL, limit[19:16]
    u8  base_high;   // 바이트 7  : base[31:24]
} __attribute__((packed));

/* lgdt에 넘기는 포인터 구조체 — 6바이트 고정 */
struct gdt_ptr {
    u16 limit; // GDT 크기 - 1
    u32 base;  // GDT 시작 주소
} __attribute__((packed));

void init_gdt();

#endif