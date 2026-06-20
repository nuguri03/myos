#include "pmm.h"

#define BITMAP_INDEX(page) ((page) / 32)
#define BITMAP_OFFSET(page) ((page) % 32)

#define MAX_PAGES           (1024 * 1024)        // 4GB / 4KB
#define BITMAP_ARRAY_SIZE   (MAX_PAGES / 32)  // 32768
#define PAGE_SIZE           4096

static u32 bitmap_data[BITMAP_ARRAY_SIZE];
static u32* bitmap = bitmap_data;

static u32 total_pages = 0;
static u32 bitmap_size = 0;

static u32 last_alloc = 0;

static inline void bitmap_set(u32 page) {
    bitmap[BITMAP_INDEX(page)] |= ((u32)1 << BITMAP_OFFSET(page));
}

static inline void bitmap_clear(u32 page) {
    bitmap[BITMAP_INDEX(page)] &= ~ ((u32)1 << BITMAP_OFFSET(page));
}

static inline bool bitmap_test(u32 page) {
    return bitmap[BITMAP_INDEX(page)] & ((u32)1 << BITMAP_OFFSET(page));
}

void* alloc_page() {
    for (u32 i = last_alloc; i < total_pages; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            last_alloc = i;
            return (void*)(i << 12);
        }
    }

    for (u32 i = 0; i < last_alloc; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            last_alloc = i;
            return (void*)(i << 12);
        }
    }
    return NULL;
}

void free_page(void* page) {
    u32 idx = (u32)page >> 12;
    bitmap_clear(idx);
}

void init_pmm(struct e820_entry* map, u32 count, u32 kernel_start, u32 kernel_end) {
    // 1. total_pages 계산: usable 영역 합산
    total_pages = 0;
    for (u32 i = 0; i < count; i++) {
        u64 end = (map[i].base + map[i].length + (PAGE_SIZE - 1)) / PAGE_SIZE;

        if (end > MAX_PAGES) {
            end = MAX_PAGES;
        }

        if ((u32)end > total_pages) {
            total_pages = (u32)end;
        }
    }

    bitmap_size = (total_pages + 31) / 32;
    if (bitmap_size > BITMAP_ARRAY_SIZE) {
        bitmap_size = BITMAP_ARRAY_SIZE;
        total_pages = MAX_PAGES;
    }

    // 2. 전부 used(1)로 초기화
    for (u32 i = 0; i < bitmap_size; i++) {
        bitmap[i] = 0xFFFFFFFF;
    }

    // 3. e820 usable 영역만 free(0)로 열기
    for (u32 i = 0; i < count; i++) {
        if (map[i].type == 1) {
            u64 start_page = map[i].base / PAGE_SIZE;
            u64 end_page   = (map[i].base + map[i].length + (PAGE_SIZE - 1)) / PAGE_SIZE;

            if (start_page > MAX_PAGES) {
                start_page = MAX_PAGES;
            }
            if (end_page > MAX_PAGES) {
                end_page = MAX_PAGES;
            }

            for (u32 page = (u32)start_page; page < (u32)end_page; page++) {
                bitmap_clear(page);
            }
        }
    }

    // 4. 커널 영역 다시 used로 마킹
    u32 kstart_page = kernel_start / PAGE_SIZE;
    u32 kend_page   = (kernel_end + PAGE_SIZE - 1) / PAGE_SIZE;  // 올림

    for (u32 page = kstart_page; page < kend_page; page++) {
        bitmap_set(page);
    }

    // 5. bitmap 영역 used로 마킹
    u32 bitmap_start = (u32)bitmap_data;
    u32 bitmap_end = bitmap_start + bitmap_size * sizeof(u32);

    u32 bstart_page = bitmap_start / PAGE_SIZE;
    u32 bend_page = (bitmap_end + PAGE_SIZE - 1) / PAGE_SIZE;

    for (u32 page = bstart_page; page < bend_page; page++) {
        bitmap_set(page);
    }

    // 6. page 0 (null guard) 항상 used
    bitmap_set(0);
}