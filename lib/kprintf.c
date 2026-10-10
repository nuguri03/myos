#include "lib/kprintf.h"
#include "driver/video.h"

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

// len은 실제 저장 길이가 아니라, 전체 출력에 필요한 길이
static void buf_putc(char *buf, size_t size, size_t *len, char c) {
    // 마지막 '\0' 자리 확보. size == 0일 때의 뺄셈도 방지.
    if (size > 0 && *len < size - 1) {
        buf[*len] = c;
    }

    // 버퍼의 용량이 작아서 짤렸더라도 len은 증가
    (*len)++;
}

/* 포맷 문자열 파싱하는 함수 */
ssize_t kvsnprintf(char *buf, size_t buf_size, const char *fmt, va_list args) {
    size_t len = 0;
    const char *s;
    char number[12]; // 32비트 십진수: 부호 1 + 숫자 10 + '\0'

    for (; *fmt; fmt++) {
        if (*fmt != '%') {
            buf_putc(buf, buf_size, &len, *fmt);
            continue;
        }

        fmt++;

        // 문자열 끝에 '%'만 있는 경우: '%'를 출력하고 종료
        if (*fmt == '\0') {
            buf_putc(buf, buf_size, &len, '%');
            break;
        }

        switch (*fmt) {
            case 'c':
                buf_putc(buf, buf_size, &len,
                         (char)va_arg(args, i32));
                break;

            case 's':
                s = va_arg(args, const char *);
                if (!s) {
                    s = "(null)";
                }
                while (*s) {
                    buf_putc(buf, buf_size, &len, *s++);
                }
                break;

            case 'd':
                itoa(va_arg(args, i32), number, 10);
                for (s = number; *s; s++) {
                    buf_putc(buf, buf_size, &len, *s);
                }
                break;

            case 'u':
                utoa(va_arg(args, u32), number, 10);
                for (s = number; *s; s++) {
                    buf_putc(buf, buf_size, &len, *s);
                }
                break;

            case 'x':
                utoa(va_arg(args, u32), number, 16);
                for (s = number; *s; s++) {
                    buf_putc(buf, buf_size, &len, *s);
                }
                break;

            case '%':
                buf_putc(buf, buf_size, &len, '%');
                break;

            default:
                buf_putc(buf, buf_size, &len, '%');
                buf_putc(buf, buf_size, &len, *fmt);
                break;
        }
    }

    if (buf_size > 0) {
        size_t end = (len < buf_size) ? len : buf_size - 1;
        buf[end] = '\0';
    }

    return (ssize_t)len;
}

ssize_t kprintf(const char* fmt, ...) {
    char buf[1024];
    va_list args;

    va_start(args, fmt);

    ssize_t result = kvsnprintf(buf, sizeof(buf), fmt, args);

    va_end(args);

    if (result >= 0) {
        size_t stored = (size_t)result;

        // 출력이 짤렸을 때
        if (stored >= sizeof(buf)) {
            stored = sizeof(buf) - 1;
        }

        vga_print(buf, stored);
    }

    return result;
}