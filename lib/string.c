#include "string.h"

/* 문자열의 길이 출력 함수 */
ssize_t strlen(const char* str) {
    if (str == NULL) {
        return -1;
    }

    ssize_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    
    return len;
}