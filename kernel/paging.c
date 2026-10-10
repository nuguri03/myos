// virtual_address -> page_directory -> page_table -> physical_address
#include "paging.h"

#define PAGE_TABLE_COUNT 5  // 5개 * 4MB = 20MB identity map

static u32 page_directory[1024] __attribute__((aligned(4096)));
static u32 page_table[PAGE_TABLE_COUNT][1024] __attribute__((aligned(4096)));

/* TLB 무효화 */
static inline void invlpg(void* addr) {
    __asm__ volatile(
        "invlpg (%0)"
        :
        : "r"(addr)
        : "memory"
    );
}

bool map_page(void* virtual_addr, void* physical_addr, u32 flags) {
    u32 virt = (u32)virtual_addr;
    u32 phys = (u32)physical_addr;

    if ((virt & 0xFFF) != 0 || (phys & 0xFFF) != 0) {
        return false; // 주소가 4KB 정렬되지 않음
    }

    u32 pd_index = virt >> 22;
    u32 pt_index = (virt >> 12) & 0x3FF;

    if (pd_index >= PAGE_TABLE_COUNT) {
        return false; // 20MB 이상 영역은 지원하지 않음
    }

    page_table[pd_index][pt_index] = phys | PAGE_PRESENT | flags;

    invlpg(virtual_addr); // TLB 무효화

    return true;
}

bool unmap_page(void* virtual_addr) {
    u32 virt = (u32)virtual_addr;

    if ((virt & 0xFFF) != 0) {
        return false; // 주소가 4KB 정렬되지 않음
    }

    u32 pd_index = virt >> 22;
    u32 pt_index = (virt >> 12) & 0x3FF;

    if (pd_index >= PAGE_TABLE_COUNT) {
        return false; // 20MB 이상 영역은 지원하지 않음
    }

    page_table[pd_index][pt_index] = 0; // 엔트리 제거

    invlpg(virtual_addr); // TLB 무효화

    return true;
}

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