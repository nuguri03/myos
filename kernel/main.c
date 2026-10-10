// kernel
#include "kernel/gdt.h"
#include "kernel/idt.h"
#include "kernel/paging.h"
#include "kernel/pmm.h"
#include "kernel/heap.h"

// driver
#include "driver/serial.h"
#include "driver/video.h"
#include "driver/pit.h"
#include "driver/keyboard.h"

// lib
#include "kstdio.h"
#include "kstdlib.h"

#define E820_COUNT_ADDR 0x4FC
#define E820_BUF_ADDR   0x500

extern u32 _kernel_start;
extern u32 _kernel_end;

void main() {
    // 디버깅용
    init_serial();

    // pmm
    struct e820_entry* map = (struct e820_entry*)E820_BUF_ADDR;
    u32 count = *(u32*)E820_COUNT_ADDR;
    init_pmm(map, count, (u32)&_kernel_start, (u32)&_kernel_end);
    serial_printf("PMM good\n");

    // paging
    init_paging();
    serial_printf("PAGING good\n");

    // heap
    init_heap();
    serial_printf("HEAP good\n");

    // heap test
    int *a = (int *)kmalloc(sizeof(int));
    *a = 42;
    if (a) {
        serial_printf("malloc good\n");
        
        kfree(a);
        serial_printf("free good\n");
    }
    else {
        serial_printf("malloc BADDDD\n");
    }

    // GDT/IDT
    init_gdt();
    init_idt();
    serial_printf("GDT/IDT good\n");
    
    // 디바이스
    init_pit(1000);
    init_keyboard();
    serial_printf("device good\n");

    // 화면 지우기
    clear_vga();
    
    // // printf example -------------------------
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