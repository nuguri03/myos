#include "selftest.h"

#include "kernel/heap.h"
#include "kernel/pmm.h"
#include "kernel/paging.h"

#include "driver/serial.h"

#include "kernel/kthread.h"
#include "kstdio.h"
#include "kstdlib.h"
#include "lib/kstring.h"

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
        void* page = alloc_page();
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

// verify shared mappings and page cleanup
void test_paging() {
    reserve_region(0x013EE000, 0x013F0000);
    void *phys = alloc_page();

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
            if (free_page(phys + 4)) serial_printf("free_page phys+4 success\n");
            else serial_printf("free_page phys+4 failed\n");

            if (free_page(phys)) serial_printf("free_page phys success\n");
            else serial_printf("free_page phys failed\n");

            if (free_page(phys)) serial_printf("free_page phys again success\n");
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

// memset test
void test_kmemset() {
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
}

// memcpy test
void test_kmemcpy() {
    unsigned char src[3] = {0x12, 0x00, 0xAB};
    unsigned char dst[5] = {1, 2, 3, 4, 5};

    void* memcpy_test = kmemcpy(dst + 1, src, sizeof(src));
    for (size_t i = 0; i < 5; i++) {
        serial_printf("%d ", dst[i]);
    }
    serial_printf("\n");
}

// memmove test
void test_kmemmove() {
    char a[] = "ABCDE";
    kmemmove(a + 1, a, 4);
    serial_printf("%s\n", a);  // AABCD — 뒤에서부터 복사

    char b[] = "ABCDE";
    kmemmove(b, b + 1, 4);
    serial_printf("%s\n", b);  // BCDEE — 앞에서부터 복사
}

// strcmp test
void test_kstrcmp() {
    char a[] = "ABCDE";
    char b[] = "ABCDE";
    char c[] = "EDCBA";
    serial_printf("%s | %s: %d\n", a, b, kstrcmp(a, b));
    serial_printf("%s | %s: %d\n", a, c, kstrcmp(a, c));
}

void test_kprintf() {
    char* msg = "Hello Kernel!";
    kprintf("%s\n%x\n", msg, 125);
    __asm__ volatile("int $0x03");
}

static struct task_struct main_task;
static struct task_struct worker_task;

static i32 worker_entry(void* data) {
    u32* shared = (u32*)data;

    for (u32 i = 1; i <= 3; i++) {
        *shared += 1;
        serial_printf("worker: %u, data=%u\n", i, *shared);
        yield();
    }

    return 42;
}

void test_thread(void) {
    sched_init(&main_task);

    u32* shared = kmalloc(sizeof(u32));
    if (shared == NULL) {
        return;
    }

    *shared = 123;

    if (!kthread_create(&worker_task, worker_entry, shared)) {
        serial_printf("FAIL: kthread_create\n");
        kfree(shared);
        return;
    }

    main_task.next = &worker_task;
    worker_task.next = &main_task;

    while (!worker_task.exited) {
        *shared += 1;
        serial_printf("main: %u\n", *shared);
        yield();
    }

    bool passed = true;

    if (!worker_task.exited || worker_task.exit_code != 42) {
        serial_printf("FAIL: worker exit state or code\n");
        passed = false;
    }

    if (main_task.next != &main_task) {
        serial_printf("FAIL: worker still linked\n");
        passed = false;
    }

    // 실행 목록에서 제거됐음을 확인한 뒤 회수
    if (main_task.next == &main_task) {
        if (!kthread_reap(&worker_task)) {
            serial_printf("FAIL: worker stack cleanup\n");
            passed = false;
        } else {
            serial_printf("worker stack reclaimed\n");

            if (kthread_reap(&worker_task)) {
                serial_printf("FAIL: accepted repeated reap\n");
                passed = false;
            }
        }
    }

    kfree(shared);

    if (passed) {
        serial_printf("PASS: thread exit and cleanup\n");
    }
}