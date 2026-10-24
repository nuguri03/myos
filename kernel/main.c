// kernel
#include "kernel/gdt.h"
#include "kernel/idt.h"
#include "kernel/paging.h"
#include "kernel/pmm.h"
#include "kernel/heap.h"
#include "kernel/kthread.h"

// driver
#include "driver/serial.h"
#include "driver/video.h"
#include "driver/pit.h"
#include "driver/keyboard.h"

// lib
#include "kstdio.h"
#include "kstdlib.h"
#include "lib/kstring.h"
#include "selftest.h"

#define E820_COUNT_ADDR 0x4FC
#define E820_BUF_ADDR   0x500

extern u32 _kernel_start;
extern u32 _kernel_end;

void main() {
    // serial
    init_serial();

    // physical memory management
    struct e820_entry* map = (struct e820_entry*)E820_BUF_ADDR;
    u32 count = *(u32*)E820_COUNT_ADDR;
    pmm_init(map, count, (u32)&_kernel_start, (u32)&_kernel_end);
    serial_printf("PMM ok\n");

    // paging
    init_paging();
    serial_printf("PAGING ok\n");
    
    // stack
    reserve_region(0x013F0000, 0x01400000); // 64KB stack

    // heap
    init_heap();
    serial_printf("HEAP ok\n");

    // GDT/IDT
    init_gdt();
    init_idt();
    serial_printf("GDT/IDT ok\n");
    
    // device
    init_pit(1000);
    serial_printf("PIT ok\n");
    init_keyboard();
    serial_printf("KEYBOARD ok\n");

    clear_vga();

    // serial_printf("--------------test--------------\n");
    // test_heap();
    
    // serial_printf("--------------test--------------\n");
    // test_heap_overlap(); // warning: this test do not free page
    
    // serial_printf("--------------test--------------\n");
    // test_paging();        
    
    // serial_printf("--------------test--------------\n");
    // test_page_fault(); // warning: cause interrupt
    
    // serial_printf("--------------test--------------\n");
    // test_thread();
    
    serial_printf("--------------test--------------\n");
    test_preemption();
    
    serial_printf("--------------------------------\n");
    
    __asm__ volatile("sti");

    serial_printf("\nkernel is working....\n");

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