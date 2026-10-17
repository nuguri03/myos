#ifndef STRING_H
#define STRING_H

#include "types.h"

ssize_t kstrlen(const char* str);
int kstrcmp(const char* a, const char* b);

void* kmemset(void* dest, int value, size_t count);
void* kmemcpy(void* dest, const void* src, size_t count);
void* kmemmove(void* dest, const void *src, size_t count);

#endif