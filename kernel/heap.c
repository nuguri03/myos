#include "kernel/heap.h"
#include "kernel/pmm.h"

extern u32 _kernel_end;

// 4KB 정렬                              

#define HEADER_SIZE sizeof(struct block_header)

struct block_header *heap = NULL;

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