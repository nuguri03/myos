#include "string.h"

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