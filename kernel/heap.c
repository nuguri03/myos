#include "heap.h"
#include "pmm.h"

extern u32 _kernel_end;

// 4KB 정렬
#define KERNEL_END_ALIGNED (((u32)&_kernel_end + 0xFFFu) & ~0xFFFu)

#define HEAP_START ((KERNEL_END_ALIGNED) < 0x00100000u ? 0x00100000u : KERNEL_END_ALIGNED)

// stack이 0x01400000 ~ 0x013F0000 사이에 위치하므로, heap은 그보다 낮은 주소에 위치해야 함
#define HEAP_END    0x013F0000                              

#define HEADER_SIZE sizeof(struct block_header)

static struct block_header *heap = NULL;

void init_heap() {
    heap = (struct block_header *)HEAP_START;
    if (HEAP_START + HEADER_SIZE > HEAP_END) {
        heap = NULL;
        return;
    }

    reserve_region(HEAP_START, HEAP_END);

    heap->size = HEAP_END - HEAP_START - HEADER_SIZE;
    heap->free = 1;
    heap->next = NULL;
}

// 인접한 free 블록 병합
static void coalesce() {
    struct block_header *cur = heap;
    while (cur && cur->next) {
        if (cur->free && cur->next->free) {
            // 현재 블록 + 헤더 + 다음 블록 크기로 합침
            cur->size += HEADER_SIZE + cur->next->size;
            cur->next = cur->next->next;
        } else {
            cur = cur->next;
        }
    }
}

void* malloc(size_t size) {
    if (size == 0) return NULL;

    // 4바이트 정렬
    size = (size + 3) & ~3;

    struct block_header *cur = heap;
    while (cur) {
        if (cur->free && cur->size >= size) {
            // 남는 공간 헤더 + 최소 4바이트 이상이 블록 분할
            if (cur->size >= size + HEADER_SIZE + 4) {
                struct block_header* new = (struct block_header*)((u8*)cur + HEADER_SIZE + size);
                new->size = cur->size - size - HEADER_SIZE;
                new->free = 1;
                new->next = cur->next;
                cur->next = new;
                cur->size = size;
            }
            cur->free = 0;
            return (void*)((u8*)cur + HEADER_SIZE);
        }
        cur = cur->next;
    }
    return NULL;
}

void free(void* ptr) {
    if (!ptr) return;

    struct block_header *header = (struct block_header *)((u8 *)ptr - HEADER_SIZE);
    header->free = 1;

    coalesce();
}