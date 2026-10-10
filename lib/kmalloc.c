#include "kernel/kmalloc.h"
#include "kernel/heap.h"

void* kmalloc(size_t size) {
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