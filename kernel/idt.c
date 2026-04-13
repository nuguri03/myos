/* IDT = Interrupt Descriptor Table
    인터럽트 번호 -> 핸들러 함수 주소를 매핑하는 테이블 */

#include "idt.h"
#include "pic.h"

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void *isr_table[256];

static void idt_set_gate(u8 num, u32 base, u16 sel, u8 flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

static void idt_load(u32 idt_ptr_addr) {
    __asm__ __volatile__("lidt (%0)" : : "r" (idt_ptr_addr));
}

void init_idt() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (u32)&idt;

    // 0x08 = code, 0x8E = 32비트 interrupt gate
    for (i32 i = 0; i < 256; i++) {
        idt_set_gate(i, (u32)isr_table[i], 0x08, 0x8E);
    }
    
    pic_remap(0x20, 0x28);

    idt_load((u32)&idtp);
}