// kernel
#include "gdt.h"
#include "idt.h"
#include "paging.h"
#include "pmm.h"
#include "heap.h"

// driver
#include "serial.h"
#include "video.h"
#include "pit.h"
#include "keyboard.h"

// lib
#include "printf.h"

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
    serial_kprintf("PMM good\n");

    // paging
    init_paging();
    serial_kprintf("PAGING good\n");

    // stack
    reserve_region(0x013F0000, 0x01400000); // 64KB stack

    // heap
    init_heap();
    serial_kprintf("HEAP good\n");

    // heap test
    int *a = (int *)malloc(sizeof(int));
    *a = 42;
    if (a) {
        serial_kprintf("malloc good\n");
        
        free(a);
        serial_kprintf("free good\n");
    }
    else {
        serial_kprintf("malloc BADDDD\n");
    }
    // heap overlap test
    // u32 heap_start = ((u32)&_kernel_end + 0xFFF) & ~0xFFF;
    // u32 heap_end = 0x01400000;

    // u32 allocated = 0;
    // bool overlap = false;

    // while (1) {
    //     void* page = alloc_page();
    //     if (page == NULL) {
    //         break;
    //     }
    //     serial_kprintf("alloc_page: %x\n", (u32)page);
        
    //     u32 addr = (u32)page;

    //     if (addr >= heap_start && addr < heap_end) {
    //         serial_kprintf("FAIL: heap overlap at %x\n", addr);
    //         overlap = true;
    //         break;
    //     }

    //     allocated++;
    // }

    // if (!overlap) {
    //     serial_kprintf("PASS: no heap overlap, pages=%u\n", allocated);
    // }

    // GDT/IDT
    init_gdt();
    init_idt();
    serial_kprintf("GDT/IDT good\n");
    
    // 디바이스
    init_pit(1000);
    init_keyboard();
    serial_kprintf("device good\n");

    // 화면 지우기
    clear_vga();
    
    // // printf example -------------------------
    // char* msg = "Hello Kernel!";
    // kprintf("%s\n%x\n", msg, 125);
    // __asm__ volatile("int $0x03");
    // // example -------------------------

    // PAGE FAULT 테스트
    // unmap_page((void*)0x00000000);
    // volatile u32 *ptr = (u32*)0x00000000;
    // *ptr = 123;

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