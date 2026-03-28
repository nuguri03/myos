#include "gdt.h"
#include "idt.h"
#include "video.h"
#include "printf.h"

void main() {
    init_gdt();
    init_idt();

    clear_vga();

    char* msg = "Hello Kernel!";
    kprintf("%s\n%x\n", msg, 125);

    __asm__ volatile("int $0x03");

    __asm__ volatile("sti");

    while (1) {}
}