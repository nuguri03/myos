// 물리 메모리 관리
#include "pmm.h"
#include "paging.h"

/* 물리 메모리 관리 (Physical Memory Manager, PMM)
* - 4KB 단위 페이지를 관리
* - 비트맵(bitmap)으로 사용 여부를 추적
* - e820 메모리 맵을 기반으로 초기화
*/

// 비트맵(bitmap)은 여러 대상의 상태를 비트 하나씩으로 기록하는 방식
// PMM에서는 물리 페이지 하나의 할당 가능 여부를 비트 하나에 저장
// u32 배열을 사용하여 배열 1개당 32개의 페이지 상태를 기록

#define BITMAP_INDEX(page)  ((page) / 32)
#define BITMAP_OFFSET(page) ((page) % 32)

#define MAX_PAGES           (1024 * 1024)        // 4GB(2^32)(32비트 운영체제의 최대 주소 공간) / 4KB(2^12)(4KB 페이지) = 1,048,576(PAGES)
#define BITMAP_ARRAY_SIZE   (MAX_PAGES / 32)     // 4GB / 4KB / 32 = 32,768 (하나의 배열 값에 32개의 페이지 상태를 기록하므로 32로 나눔)

static u32 bitmap_data[BITMAP_ARRAY_SIZE];
static u32* bitmap = bitmap_data;

static u32 total_pages = 0;
static u32 bitmap_size = 0;

static u32 last_alloc = 0;

/* example(37th page)
* 37th page -> 37 / 32 = 1 (bitmap[1]에 저장)
* 37 % 32 = 5 (bitmap[1]의 5번째 비트에 저장)
*/

static inline void bitmap_set(u32 page) {
    bitmap[BITMAP_INDEX(page)] |= ((u32)1 << BITMAP_OFFSET(page));
}

static inline void bitmap_clear(u32 page) {
    bitmap[BITMAP_INDEX(page)] &= ~ ((u32)1 << BITMAP_OFFSET(page));
}

static inline bool bitmap_test(u32 page) {
    // bool == char 이라서 != 0 으로 비교 안하면 앞에 짤려서 8비트만 비교됨
    return (bitmap[BITMAP_INDEX(page)] & ((u32)1 << BITMAP_OFFSET(page))) != 0;
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

void reserve_region(u32 start, u32 end) {
    if (start >= end) {
        return;
    }

    u32 start_page = start / PAGE_SIZE;
    u32 end_page = (u32)(((u64)end + PAGE_SIZE - 1) / PAGE_SIZE);

    if (end_page > total_pages) {
        end_page = total_pages;
    }

    for (u32 page = start_page; page < end_page; page++) {
        bitmap_set(page);
    }
}

void init_pmm(struct e820_entry* map, u32 count, u32 kernel_start, u32 kernel_end) {
    // 1. total_pages 계산 및 bitmap_size 계산: usable 영역 합산
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
    reserve_region(kernel_start, kernel_end);

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