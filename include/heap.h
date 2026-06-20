#ifndef HEAP_H
#define HEAP_H

#include "types.h"

struct block_header {
    u32 size;
    u32 free;
    struct block_header* next;
};

void init_heap();

void* malloc(size_t size);
void free(void* ptr);

#endif