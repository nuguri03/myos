#ifndef KSTDLIB_H
#define KSTDLIB_H

#include "types.h"

void* kmalloc(size_t size);
void kfree(void* ptr);

#endif