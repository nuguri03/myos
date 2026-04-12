#include "string.h"

/* 문자열의 길이 출력 함수 */
i32 strlen(const char* str) {
    if (str == 0) {
        return -1;
    }

    i32 len = 0;
    while (str[len] != '\0') {
        len++;
    }
    
    return len;
}