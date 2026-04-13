#include "gdt.h"

struct gdt_entry gdt[3];
struct gdt_ptr gp;

static void gdt_flush(u32 gdt_ptr_addr) {
    __asm__ __volatile__(
        "lgdt (%0)\n\t"      
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "mov %%ax, %%ss\n\t"
        "pushl $0x08\n\t" 
        "pushl $1f\n\t"   
        "lret\n\t"        
        "1:\n\t"          
        :
        : "r"(gdt_ptr_addr)
        : "eax", "memory"  
    );
}

/* base(32비트)와 limit(20비트)를 Intel GDT 스펙에 맞게 쪼개서 저장 */
static void gdt_set_gate(i32 num, u32 base, u32 limit, u8 access, u8 granularity) {
    // base를 low(16비트), middle(8비트), high(8비트)로 분해
    gdt[num].base_low    = base & 0xFFFF;
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;

    // limit을 low(16비트)와 granularity 하위 4비트로 분해
    gdt[num].limit_low   = limit & 0xFFFF;
    gdt[num].granularity = (limit >> 16) & 0x0F;

    // granularity 상위 4비트에 플래그(G, D/B, L, AVL) 합성
    gdt[num].granularity |= granularity & 0xF0;

    gdt[num].access = access;
}

void init_gdt() {
    // sizeof(null + code + data) - 1 
    gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gp.base = (u32)&gdt;

    // null descripter
    gdt_set_gate(0, 0, 0, 0, 0);

    // code
    // 0x9A = 1001 1010
    // P=1, DPL=00, S=1, E=1, DC=0, RW=1, A=0
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // data
    // 0x92 = 1001 0010
    // P=1, DPL=00, S=1, E=0, DC=0, RW=1, A=0
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    gdt_flush((u32)&gp);
}