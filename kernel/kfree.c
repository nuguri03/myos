#include "kernel/kfree.h"
#include "kernel/heap.h"

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

void kfree(void* ptr) {
    if (!ptr) return;

    struct block_header *header = (struct block_header *)((u8 *)ptr - HEADER_SIZE);
    header->free = 1;

    coalesce();
}