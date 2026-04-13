#ifndef TYPES_H
#define TYPES_H

// TODO: int가 32비트임을 보장 할 수 없다. 따라서 #include <stdint.h>를 사용하자.
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;

typedef signed char         i8;
typedef signed short        i16;
typedef signed int          i32;

#define true 1
#define false 0

#endif