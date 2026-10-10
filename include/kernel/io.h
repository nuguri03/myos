#ifndef IO_H
#define IO_H

#include "types.h"

// 포트에 데이터 쓰기
static inline void outb(u16 port, u8 data) {
    // al 레지스터 값을 port번 I/O 주소로 전송
    // outb al, dx 
    // outb al, imm8 (imm8 <= 0xFF)
    // "a" = eax, ax, al 레지스터에 넣어라
    // "dN" = 0~0xFF 범위면 즉시값(imm8) 사용 가능
    __asm__ __volatile__ ("outb %0, %1": : "a"(data), "dN"(port));
}

// 포트에서 데이터 읽기
static inline u8 inb(u16 port) {
    // "=a" = al 레지스터 값을 변수에 저장
    u8 ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

static inline void io_wait() {
    // 0x80 = POST 진단 포트
    // 아무 의미 없는 쓰기를 통해서 시간을 태우는 용도
    // cpu가 레거시 칩보다 속도가 빠르기 때문에 필요
    outb(0x80, 0);
}

#endif