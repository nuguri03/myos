[BITS 32]

extern isr_handler

%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli                     ; 인터럽트 중첩 방지
    push byte 0             ; 에러 코드가 없는 예외는 0으로 채워서 스택 구조를 통일시킴
    push byte %1            ; 인터럽트 번호 push
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli                     ; 인터럽트 중첩 방지
                            ; 에러 코드를 CPU가 자동으로 push했음으로 생략
    push byte %1            ; 인터럽트 번호 push
    jmp isr_common_stub
%endmacro

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31
ISR_NOERRCODE 32
ISR_NOERRCODE 33
ISR_NOERRCODE 34
ISR_NOERRCODE 35
ISR_NOERRCODE 36
ISR_NOERRCODE 37
ISR_NOERRCODE 38
ISR_NOERRCODE 39
ISR_NOERRCODE 40
ISR_NOERRCODE 41
ISR_NOERRCODE 42
ISR_NOERRCODE 43
ISR_NOERRCODE 44
ISR_NOERRCODE 45
ISR_NOERRCODE 46
ISR_NOERRCODE 47

; 문맥 저장 및 핸들러 호출
isr_common_stub:
    pusha           ; 범용 + 스택 + 인덱스 레지스터 백업
    xor eax, eax    
    mov ax, ds
    push eax        ; ds 레지스터 백업
    mov ax, 0x10    ; 커널 데이터 세그먼트로 전환(인터럽트 발생 시점이 유저 세그먼트 일 수도 있음)
    mov ds, ax  
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp        ; 현재 스택 포인터 = struct *registers 로 전달
    call isr_handler
    add esp, 4      ; pop

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa            ; pusha로 저장한 레지스터 모두 복원
    add esp, 8      ; int_no + error_code (각 4바이트) pop

    iret            ; 스택에서 EIP, CS, EFLAGS 복원