#include "printf.h"
#include "stdarg.h"
#include "video.h"

/* 정수를 문자열로 변경하는 함수
base는 10 또는 16으로 사용 */
static size_t utoa(u32 value, char *buf, u32 base) {
    static const char digits[] = "0123456789ABCDEF";
    size_t len = 0;

    do {
        buf[len++] = digits[value % base];
        value /= base;
    } while (value != 0);

    // 버퍼 반전(019 -> 910)
    for (size_t i = 0; i < len / 2; i++) {
        char tmp = buf[i];
        buf[i] = buf[len - 1 - i];
        buf[len - 1 - i] = tmp;
    }

    buf[len] = '\0';
    return len;
}

static size_t itoa(i32 value, char *buf, u32 base) {
    if (base == 10 && value < 0) {
        buf[0] = '-';

        // INT_MIN도 처리하도록 unsigned 연산으로 절댓값 계산
        return 1 + utoa(0u - (u32)value, buf + 1, base);
    }

    return utoa((u32)value, buf, base);
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

/* 포맷 문자열 파싱하는 함수 */
static ssize_t vsprintf(char* buf, const char *fmt, va_list args) {
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

            case 'u':
                str += itoa(va_arg(args, u32), str, 10);
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

ssize_t kprintf(const char* fmt, ...) {
    char buf[1024];
    va_list args;
    ssize_t written;

    va_start(args, fmt);

    written = vsprintf(buf, fmt, args);

    va_end(args);

    if (written < 0) {
        return written;
    }

    vga_print(buf, written);

    return written;
}