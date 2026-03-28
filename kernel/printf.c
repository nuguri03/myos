#include "printf.h"
#include "stdarg.h"
#include "video.h"

static i32 itoa(const i32 number, char* str, u32 base) {
    u32 i = 0;
    bool is_negative = false;

    if (number == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return i;
    }

    u32 u_number;

    if (base == 10 && number < 0) {
        is_negative = true;
        str[i++] = '-';
        u_number = (u32)(-number);
    } else {
        u_number = (u32)number;
    }

    while (u_number != 0) {
        u32 rem = u_number % base;

        str[i++] = (rem > 9) ? (rem - 10) + 'A' : rem + '0';
        u_number = u_number / base;
    }

    i32 start = is_negative ? 1 : 0;
    i32 end = i - 1;
    while (start < end) {
        u8 temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
    str[i] = '\0';

    return i;
}
// 언젠가는 쓸 거임: kprintf("%5d", 10); 이런거 추가 할 때 사용할 듯
// static i32 atoi(const char* str) {
//     i32 number = 0;
//     i32 sign = 1;

//     if (*str == '-' || *str == '+') {
//         if (*str == '-') {
//             sign = -1;
//         }
//         str++;
//     }

//     while ('0' <= *str && *str <= '9') {
//         number = number * 10 + (*str - '0');
//         str++;
//     }

//     return number * sign;
// }

static i32 vsprintf(char* buf, const char *fmt, va_list args) {
    char *str = buf;
    const char* s;

    for (; *fmt; fmt++) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }

        fmt++;

        switch (*fmt) {
            case 'c':
                *str++ = (u8)va_arg(args, i32);
                break;

            case 's':
                s = va_arg(args, char*);
                if (!s) return -1;
                while (*s) {
                    *str++ = *s++;
                }
                break;

            case 'd':
                str += itoa(va_arg(args, i32), str, 10);
                break;

            case 'x':
                str += itoa(va_arg(args, i32), str, 16);
                break;

            case '%':
                *str++ = '%';
                break;

            default:
                *str++ = '%';
                *str++ = *fmt;
                break;
        }
    }

    *str = '\0';
    return str - buf;
}

i32 kprintf(const char* fmt, ...) {
    char buf[1024];
    va_list args;
    i32 written;

    va_start(args, fmt);

    written = vsprintf(buf, fmt, args);

    va_end(args);

    vga_print_string(buf, written);
}