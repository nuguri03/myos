#ifndef PMM_H
#define PMM_H

#include "types.h"

typedef struct {
    u64 base;     // 이 영역의 시작 물리 주소
    u64 length;   // 이 영역의 크기 (바이트)
    u32 type;     // 영역의 종류
    u32 acpi;     // ACPI 확장 속성 (대부분 무시)
} __attribute__((packed)) e820_entry_t;

void* alloc_page();
void free_page(void* page);

void init_pmm(e820_entry_t* map, u32 count, u32 kernel_start, u32 kernel_end);

#endif