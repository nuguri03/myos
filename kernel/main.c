#include "gdt.h"
#include "idt.h"
#include "pit.h"
#include "video.h"
#include "printf.h"

void main() {
    init_gdt();
    init_idt();
    init_pit(1000);

    clear_vga();

    // example -------------------------
    char* msg = "Hello Kernel!";
    kprintf("%s\n%x\n", msg, 125);
    __asm__ volatile("int $0x03");
    // example -------------------------

    __asm__ volatile("sti");

    while (1) {
        kprintf("%u\n", pit_get_ticks());
    }
}