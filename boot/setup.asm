[BITS 16]

%define KERNEL_CODE_SEG 0x08
%define KERNEL_DATA_SEG 0x10

%define E820_MAGIC      0x534D4150
%define E820_ENTRY_SIZE 24
%define E820_BUF_ADDR   0x500
%define E820_COUNT_ADDR 0x4FC

global setup_start ; 링커의 엔트리 포인트
extern main

; ==GDT(Global Descriptor Table)=============================  
; 메모리 구역마다 권한과 크기를 정함으로써 
; 메모리를 보호하고, 32비트(4GB) 전체를 사용하기 위한 테이블
; ===========================================================
setup_start:
    cli             ; 모든 인터럽트를 중단
    ; GDT가 완성되지 않아 시스템 불안정
    ; Stack이 정의되지 않음
    ; 원자성 보장


    ; == E820 메모리 맵 수집 ====================================
    ; BIOS int 0x15 / eax=0xE820 호출
    ; es:di → 엔트리를 저장할 버퍼
    ; ebx   → 0으로 시작, BIOS가 다음 엔트리 위치를 여기에 써줌
    ;         다음 호출 때 그대로 넘기면 됨, 0이 되면 마지막
    ; ecx   → 엔트리 크기 (24바이트 요청)
    ; edx   → 매직넘버 0x534D4150 ("SMAP")
    ; 반환: CF=1이면 에러 or 끝, ebx=0이면 마지막 엔트리
    ; ===========================================================
    xor ax, ax
    mov es, ax
    mov di, E820_BUF_ADDR
    xor ebx, ebx
    xor ebp, ebp

.e820_loop:
    mov eax, 0xE820
    mov ecx, E820_ENTRY_SIZE
    mov edx, E820_MAGIC
 

    jc .e820_done       ; CF=1 -> 에러 or 마지막 엔트리 이후
    cmp eax, E820_MAGIC
    jne .e820_done      ; eax에 "SMAP" 없으면 이 BIOS는 E820 미지원

    test ecx, ecx       ; ecx = 0이면 빈 엔트리
    jz .e820_next

    inc ebp             ; ebp += 1
    add di, E820_ENTRY_SIZE ; di += 24, 다음 슬롯으로

.e820_next:
    test ebx, ebx       ; ebx = 0이면 마지막 엔트리
    jz .e820_done
    jmp .e820_loop

.e820_done:
    mov dword [E820_COUNT_ADDR], ebp    ; E820_COUNT_ADDR에 엔트리 개수 저장

    ; gdtr 메모리 주소로 가서 GDT를 읽고 GDT 레지스터에 저장
    ; size(2byte) / offset(4byte): gdt가 메모리 어디에 위치하는지
    lgdt [gdtr]


    ; cr0 = 0 : 16bit real mode
    ; cr0.pe = 1 ; 32bit protected mode
    mov eax, cr0
    or eax, 0x01
    mov cr0, eax

    ; dword(double word) = 32bit
    ; CS 레지스터를 0x08(코드 세그먼트)로 변경하고, 즉 GDT의 1번 규칙을 사용한다.
    ; 32비트 주소로 점프하면서, CPU 파이프라인에 남아있던 16비트 쓰레기 명령어들을 모두 비움.
    jmp dword KERNEL_CODE_SEG:protected_mode_entry

; ===========================================
; GDT entry 
;   Base (32bit)        : 세그먼트의 시작 주소
;   Limit (20bit)       : 세그먼트의 크기
;   Access Byte (8bit)  : 권한, 타입 등
;   Flags (4bit)        : 크기 단위, 모드 등
; ===========================================
gdt_start:
    ; NULL Descriptor
    dd 0x00000000, 0x00000000

    ; Code Segment: Base=0x0, Limit=0xFFFFF, Access=0x9A, Flags=0xC
    ;   → 0번지부터 4GB, 실행 가능, 커널 권한(Ring 0), 32비트
    dd 0x0000FFFF, 0x00CF9A00

    ; Data Segment: Base=0x0, Limit=0xFFFFF, Access=0x92, Flags=0xC
    ;   → 0번지부터 4GB, 읽기/쓰기 가능, 커널 권한(Ring 0), 32비트
    dd 0x0000FFFF, 0x00CF9200
gdt_end:

gdtr:
    ; dw = 2byte / dd = 4byte
    dw gdt_end - gdt_start - 1  ; limit
    dd gdt_start                ; base address

[BITS 32]

protected_mode_entry:
    ; 세그먼트 레지스터들을 0x10(데이터 세그먼트), 즉 GDT의 2번 규칙을 적용하게 함
    mov ax, KERNEL_DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; 스택 포인터와, 베이스 포인터의 위치를 0x90000으로 하고,
    ; 스택은 밑으로 내려가는 방향으로
    mov esp, 0x90000

    ; 커널을 부름
    call main

    ; 커널이 예기치 않게 종료 되었을 때 시스템을 정지 시킴
    jmp $