#ifndef STDARG_H
#define STDARG_H

// 주소를 바이트 단위로 다루기 위한 선언
typedef char *va_list;

/* 첫번째 가변 인자 주소 
    (char*)&n + sizeof(n), 마지막 고정 인자의 주소에 크기 만큼을 더함 */
#define va_start(ap, n) (ap = (va_list) & n + sizeof(n))

/* 다음번째 가변 인자 주소로 이동 
    ap를 sizeof(type)만큼 앞으로 이동 한 뒤 이동 전 주소 값 반환 */
#define va_arg(ap, type) (*(type *)((ap += sizeof(type)) - sizeof(type)))

/* 가변 인자 사용 종료 - ap를 NULL로 초기화 */
#define va_end(ap) (ap = (va_list)0)

#endif