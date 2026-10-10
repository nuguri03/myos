#include "kernel/heap.h"

extern u32 _kernel_end;

struct block_header *heap = NULL;

void init_heap() {
    heap = (struct block_header *)HEAP_START;
    if (HEAP_START + HEADER_SIZE > HEAP_END) {
        heap = NULL;
        return;
    }
    heap->size = HEAP_END - HEAP_START - HEADER_SIZE;
    heap->free = 1;
    heap->next = NULL;
}