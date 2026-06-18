#include "paging.h"

#define PAGE_PRESENT    (1 << 0)
#define PAGE_WRITABLE   (1 << 1)
#define PAGE_USER       (1 << 2)

static u32 page_directory[1024] __attribute__((aligned(4096)));
static u32 page_table[1024] __attribute__((aligned(4096)));

void init_paging() {
    // PD 전체 초기화
    for (u32 i = 0; i < 1024; i++) {
        page_directory[i] = 0;
    }

    // 0x00000000 ~ 0x003FFFFF identity mapping (4MB)
    for (u32 i = 0; i < 1024; i++) {
        page_table[i] = (i * 0x1000) | PAGE_PRESENT | PAGE_WRITABLE;
    }

    // PD[0]에 Page Table 등록
    page_directory[0] = (u32)page_table | PAGE_PRESENT | PAGE_WRITABLE;

    // cr3 = PD 주소
    __asm__ volatile("mov %0, %%cr3" :: "r"(page_directory));

    // cr0 pg 비트 set
    u32 cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= (1 << 31);
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
}