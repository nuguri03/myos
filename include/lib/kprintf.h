#ifndef KPRINTF_H
#define KPRINTF_H

#include "types.h"
#include "kstdarg.h"

ssize_t kvsnprintf(char* buf, size_t buf_size, const char *fmt, va_list args);

ssize_t kprintf(const char *fmt, ...);

#endif