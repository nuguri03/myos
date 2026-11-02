#include "selftest.h"
#include "kernel/paging.h"
#include "kernel/pmm.h"
#include "driver/serial.h"

// verify shared mappings and page cleanup
void test_paging() {
    reserve_region(0x013EE000, 0x013F0000);
    void *phys = page_alloc();

    if (phys != NULL) {
        volatile u32 * a = (volatile u32*)0x013EE000;
        volatile u32 * b = (volatile u32*)0x013EF000;

        bool mapped_a = map_page((void*)a, phys, PAGE_WRITABLE);
        bool mapped_b = map_page((void*)b, phys, PAGE_WRITABLE);

        if (mapped_a && mapped_b) {
            *a = 123;
            serial_printf("phys=%x a=%u, b=%u\n", phys, *a, *b);

            void *translated;
            translate_address((void*)a + 4, (&translated));
            serial_printf("phys after translate a+4=%x\n", (u32)translated);

            unmap_page((void*)a);
            serial_printf("b after unmap a, b=%u\n", *b);
                        
            void* result = (void*)0xDEADBEEF;
            bool found = translate_address((void*)a, &result);

            serial_printf("found=%u result=%x\n",
                        (u32)found, (u32)result);

            unmap_page((void*)b);
            if (page_free(phys + 4)) serial_printf("free_page phys+4 success\n");
            else serial_printf("free_page phys+4 failed\n");

            if (page_free(phys)) serial_printf("free_page phys success\n");
            else serial_printf("free_page phys failed\n");

            if (page_free(phys)) serial_printf("free_page phys again success\n");
            else serial_printf("free_page phys again failed\n");

        } else {
            serial_printf("mapping failed\n");
        }
    }
}

// PAGE FAULT test
void test_page_fault() {
    unmap_page((void*)0x00000000);
    volatile u32 *ptr = (u32*)0x00000000;
    *ptr = 123;
}