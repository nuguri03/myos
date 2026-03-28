#include "gdt.h"
#include "idt.h"
#include "video.h"
#include "printf.h"

void main() {
    init_gdt();
    init_idt();

    clear_screen();

    char* msg = "Hello Kernel!";

    kprintf("%s\n%x\n", msg, 125);
    
    while (1) {}
}