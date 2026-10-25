#include "kernel/heap.h"
#include "kernel/pmm.h"

#include "driver/serial.h"

extern u32 _kernel_end;

// 4KB 정렬                              

#define HEADER_SIZE sizeof(struct block_header)

struct block_header *heap = NULL;

bool heap_init() {
    heap = NULL;

    if (HEAP_START >= HEAP_END || HEAP_START + HEADER_SIZE > HEAP_END) {
        return false;
    }
    
    if (!reserve_region(HEAP_START, HEAP_END)) {
        return false;
    }

    heap = (struct block_header *)HEAP_START;
    heap->size = HEAP_END - HEAP_START - HEADER_SIZE;
    heap->free = 1;
    heap->next = NULL;

    return true;
}