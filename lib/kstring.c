#include "lib/kstring.h"

/* 문자열의 길이 출력 함수 */
ssize_t kstrlen(const char* str) {
    if (str == NULL) {
        return -1;
    }

    ssize_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    
    return len;
}

int kstrcmp(const char* a, const char* b) {
    const unsigned char* left = (const unsigned char*)a;
    const unsigned char* right = (const unsigned char*)b;

    for (size_t i = 0;; i++) {
        if (left[i] - right[i] != 0) {
            return (int)left[i] - (int)right[i];
        }

        if (left[i] == '\0') {
            return 0;
        }
    }
}

void* kmemset(void* dest, int value, size_t count) {
    unsigned char* bytes = dest;

    for (size_t i = 0; i < count; i++) {
        bytes[i] = value;
    }

    return dest;
}

void* kmemcpy(void* dest, const void* src, size_t count) {
    unsigned char* dst = dest;
    const unsigned char* source = src;

    for (size_t i = 0; i < count; i++) {
        *(dst + i) = *(source + i);
    }

    return dest;
}

void* kmemmove(void* dest, const void *src, size_t count) {
    unsigned char* dst = dest;
    const unsigned char* source = src;

    if ((u32)dst < (const u32)source) {
        for (size_t i = 0; i < count; i++) {
            *(dst + i) = *(source + i);
        }   
    } else {
        for (size_t i = count; i > 0; i--) {
            *(dst + i - 1) = *(source + i - 1);
        }
    }

    return dest;
}