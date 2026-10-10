#ifndef PAGING_H
#define PAGING_H

#include "types.h"

#define PAGE_SIZE           4096

#define PAGE_PRESENT     (1 << 0)
#define PAGE_WRITABLE    (1 << 1)
#define PAGE_USER        (1 << 2)

void init_paging();

bool map_page(void* virtual_addr, void* physical_addr, u32 flags);
bool unmap_page(void* virtual_addr);

#endif