#ifndef HEAP_H
#define HEAP_H

#include "types.h"

// 4KB 정렬
#define KERNEL_END_ALIGNED (((u32)&_kernel_end + 0xFFFu) & ~0xFFFu)
// 커널 끝과 1MiB 중 더 높은 주소를 힙 시작점으로 사용.
// 1MiB 아래의 BIOS/VGA 등 예약 영역과 커널 메모리를 피한다.
#define HEAP_START ((KERNEL_END_ALIGNED) < 0x00100000u ? 0x00100000u : KERNEL_END_ALIGNED)
// stack이 0x01400000 ~ 0x013F0000 사이에 위치하므로, heap은 그보다 낮은 주소에 위치해야 함
#define HEAP_END    0x013F0000

#define HEADER_SIZE sizeof(struct block_header)

extern struct block_header *heap;

struct block_header {
    u32 size;
    u32 free;
    struct block_header* next;
};

void init_heap();

void free(void* ptr);

#endif