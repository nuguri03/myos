#ifndef HEAP_H
#define HEAP_H

#include "types.h"

#define HEAP_START  (((u32)&_kernel_end + 0xFFF) & ~0xFFF)  // 4KB 정렬
#define HEAP_END    0x01400000  // 20MB

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