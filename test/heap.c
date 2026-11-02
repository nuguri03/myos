#include "selftest.h"
#include "kernel/heap.h"
#include "kernel/pmm.h"
#include "driver/serial.h"
#include "kstdlib.h"

// heap test
void test_heap() {
    int *malloc_test = (int *)kmalloc(sizeof(int));
    if (malloc_test != NULL) {
        *malloc_test = 42;
        serial_printf("malloc ok & ");

        kfree(malloc_test);
        serial_printf("free ok\n");
    } else {
        serial_printf("malloc ERROR\n");
    }
}

void test_heap_overlap() {
    extern u32 _kernel_end;

    u32 heap_start = HEAP_START;
    u32 heap_end = HEAP_END;

    u32 allocated = 0;
    bool overlap = false;

    while (1) {
        void* page = page_alloc();
        if (page == NULL) {
            break;
        }
        // serial_printf("alloc_page: %x\n", (u32)page);
        
        u32 addr = (u32)page;

        if (addr >= heap_start && addr < heap_end) {
            serial_printf("FAIL: heap overlap at %x\n", addr);
            overlap = true;
            break;
        }

        allocated++;
    }

    if (!overlap) {
        serial_printf("PASS: no heap overlap, pages=%u\n", allocated);
    }
}