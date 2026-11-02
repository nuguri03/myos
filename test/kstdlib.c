// test/kstring.c
#include "selftest.h"
#include "lib/kstring.h"
#include "driver/serial.h"

// memset test
void test_kmemset() {
    unsigned char buf[5] = {10, 20, 30, 40, 50};
    void* memset_test = kmemset(buf + 1, 0xAB, 3);

    for (size_t i = 0; i < 5; i++) {
        serial_printf("%d ", buf[i]);
    }
    serial_printf("\n");

    if (memset_test == buf + 1) {
        serial_printf("kmemset ok\n");
    } else {
        serial_printf("kmemset ERROR\n");
    }
}

// memcpy test
void test_kmemcpy() {
    unsigned char src[3] = {0x12, 0x00, 0xAB};
    unsigned char dst[5] = {1, 2, 3, 4, 5};

    void* memcpy_test = kmemcpy(dst + 1, src, sizeof(src));
    for (size_t i = 0; i < 5; i++) {
        serial_printf("%d ", dst[i]);
    }
    serial_printf("\n");
}

// memmove test
void test_kmemmove() {
    char a[] = "ABCDE";
    kmemmove(a + 1, a, 4);
    serial_printf("%s\n", a);  // AABCD — 뒤에서부터 복사

    char b[] = "ABCDE";
    kmemmove(b, b + 1, 4);
    serial_printf("%s\n", b);  // BCDEE — 앞에서부터 복사
}

// strcmp test
void test_kstrcmp() {
    char a[] = "ABCDE";
    char b[] = "ABCDE";
    char c[] = "EDCBA";
    serial_printf("%s | %s: %d\n", a, b, kstrcmp(a, b));
    serial_printf("%s | %s: %d\n", a, c, kstrcmp(a, c));
}