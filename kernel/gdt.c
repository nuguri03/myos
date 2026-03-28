#include "gdt.h"

struct gdt_entry gdt[3];
struct gdt_ptr gp;

static void gdt_flush(u32 gdt_ptr_address) {
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
        : "r"(gdt_ptr_address)
        : "eax", "memory"  
    );
}

static void gdt_set_gate(i32 num, u32 base, u32 limit, u8 access, u8 gran) {
    gdt[num].base_low = base & 0xFFFF;
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = limit & 0xFFFF;
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= gran & 0xF0;
    
    gdt[num].access = access;
}

void init_gdt() {
    gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gp.base = (u32)&gdt;

    // null descripter
    gdt_set_gate(0, 0, 0, 0, 0);

    // code
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // data
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    gdt_flush((u32)&gp);
}