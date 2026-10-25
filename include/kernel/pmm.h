#ifndef PMM_H
#define PMM_H

#include "types.h"

struct e820_entry {
    u64 base;     // 이 영역의 시작 물리 주소
    u64 length;   // 이 영역의 크기 (바이트)
    u32 type;     // 영역의 종류
    u32 acpi;     // ACPI 확장 속성 (대부분 무시)
} __attribute__((packed));

struct pmm_stats {
    u32 usable_pages;
    u32 reserved_pages;
    u32 allocated_pages;
    u32 free_pages;
};

void* page_alloc();
bool page_free(void* page);

// [start, end) 영역을 used로 마킹
bool reserve_region(u32 start, u32 end);

bool pmm_init(struct e820_entry* map, u32 count, u32 kernel_start, u32 kernel_end);

const struct pmm_stats* pmm_get_stats();

#endif