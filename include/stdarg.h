#ifndef STDARG_H
#define STDARG_H

typedef char *va_list;

#define va_start(ap, n) (ap = (va_list) & n + sizeof(n))

#define va_arg(ap, type) (*(type *)((ap += sizeof(type)) - sizeof(type)))

#define va_end(ap) (ap = (va_list)0)

#endif