#include "gdt.h"
#include "idt.h"
#include "pit.h"
#include "keyboard.h"
#include "serial.h"
#include "video.h"
#include "printf.h"

void main() {
    init_gdt();
    init_idt();
    init_pit(1000);
    init_keyboard();
    init_serial();

    clear_vga();
    
    serial_print("serial test\n");

    // // example -------------------------
    // char* msg = "Hello Kernel!";
    // kprintf("%s\n%x\n", msg, 125);
    // __asm__ volatile("int $0x03");
    // // example -------------------------

    __asm__ volatile("sti");

    while (1) {
        char c = keyboard_getchar();
        if (c) {
            vga_putchar(c);
        }
        else {
            __asm__ volatile("hlt");
        }
    }
}