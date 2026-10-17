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
#include "lib/kstring.h"

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
    serial_printf("PMM ok\n");

    // paging
    init_paging();
    serial_printf("PAGING ok\n");

    // stack
    reserve_region(0x013F0000, 0x01400000); // 64KB stack

    // heap
    init_heap();
    serial_printf("HEAP ok\n");

    // heap test
    int *malloc_test = (int *)kmalloc(sizeof(int));
    if (malloc_test != NULL) {
        *malloc_test = 42;
        serial_printf("malloc ok\n");

        kfree(malloc_test);
        serial_printf("free ok\n");
    } else {
        serial_printf("malloc ERROR\n");
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

    // // test(paging): verify shared mappings and page cleanup
    // reserve_region(0x013EE000, 0x013F0000);
    // void *phys = alloc_page();

    // if (phys != NULL) {
    //     volatile u32 * a = (volatile u32*)0x013EE000;
    //     volatile u32 * b = (volatile u32*)0x013EF000;

    //     bool mapped_a = map_page((void*)a, phys, PAGE_WRITABLE);
    //     bool mapped_b = map_page((void*)b, phys, PAGE_WRITABLE);

    //     if (mapped_a && mapped_b) {
    //         *a = 123;
    //         serial_printf("phys=%x a=%u, b=%u\n", phys, *a, *b);

    //         void *translated;
    //         translate_address((void*)a + 4, (&translated));
    //         serial_printf("phys after translate a+4=%x\n", (u32)translated);

    //         unmap_page((void*)a);
    //         serial_printf("b after unmap a, b=%u\n", *b);
                        
    //         void* result = (void*)0xDEADBEEF;
    //         bool found = translate_address((void*)a, &result);

    //         serial_printf("found=%u result=%x\n",
    //                     (u32)found, (u32)result);

    //         unmap_page((void*)b);
    //         if (free_page(phys + 4)) serial_printf("free_page phys+4 success\n");
    //         else serial_printf("free_page phys+4 failed\n");

    //         if (free_page(phys)) serial_printf("free_page phys success\n");
    //         else serial_printf("free_page phys failed\n");

    //         if (free_page(phys)) serial_printf("free_page phys again success\n");
    //         else serial_printf("free_page phys again failed\n");

    //     } else {
    //         serial_printf("mapping failed\n");
    //     }
    // }

    // GDT/IDT
    init_gdt();
    init_idt();
    serial_printf("GDT/IDT ok\n");
    
    // 디바이스
    init_pit(1000);
    init_keyboard();
    serial_printf("device ok\n");

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

    // memset test
    unsigned char buf[5] = {10, 20, 30, 40, 50};
    void* memset_test = kmemset(buf + 1, 0xAB, 3);

    for (size_t i = 0; i < 5; i++) {
        serial_printf("%d ", buf[i]);
    }
    serial_printf("\n");

    if (memset_test == buf + 1) {
        serial_printf("kmemset ok\n");
    } else {
        serial_printf("kmemset ERROR\n");
    }

    // memcpy test
    unsigned char src[3] = {0x12, 0x00, 0xAB};
    unsigned char dst[5] = {1, 2, 3, 4, 5};

    void* memcpy_test = kmemcpy(dst + 1, src, sizeof(src));
    for (size_t i = 0; i < 5; i++) {
        serial_printf("%d ", dst[i]);
    }
    serial_printf("\n");

    // memmove test
    char a[] = "ABCDE";
    kmemmove(a + 1, a, 4);
    serial_printf("%s\n", a);  // AABCD — 뒤에서부터 복사

    char b[] = "ABCDE";
    kmemmove(b, b + 1, 4);
    serial_printf("%s\n", b);  // BCDEE — 앞에서부터 복사

    // strcmp test
    serial_printf("%s | %s: %d\n", a, b, kstrcmp(a, b));

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