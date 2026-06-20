#include "paging.h"

#define PAGE_PRESENT     (1 << 0)
#define PAGE_WRITABLE    (1 << 1)

#define PAGE_TABLE_COUNT 5  // 5개 * 4MB = 20MB identity map

static u32 page_directory[1024] __attribute__((aligned(4096)));
static u32 page_table[PAGE_TABLE_COUNT][1024] __attribute__((aligned(4096)));

void init_paging() {
    // PD 전체를 not-present로 초기화
    for (u32 i = 0; i < 1024; i++) {
        page_directory[i] = 0;
    }

    // 0x00000000 ~ 0x013FFFFF (20MB) identity mapping
    // identity map: 가상 주소 == 물리 주소
    for (u32 t = 0; t < PAGE_TABLE_COUNT; t++) {
        for (u32 i = 0; i < 1024; i++) {
            // t번째 테이블의 i번째 엔트리 → 물리 주소 (t*1024 + i) * 4KB
            page_table[t][i] = ((t * 1024 + i) * 0x1000) | PAGE_PRESENT | PAGE_WRITABLE;
        }

        // PD[t]에 t번째 page table의 물리 주소 등록
        page_directory[t] = (u32)page_table[t] | PAGE_PRESENT | PAGE_WRITABLE;
    }

    // CR3에 page directory 물리 주소 로드
    __asm__ volatile("mov %0, %%cr3" :: "r"(page_directory));

    // CR0의 PG 비트(31번) set → 페이징 활성화
    u32 cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= (1 << 31);
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
}