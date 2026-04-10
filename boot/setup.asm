[BITS 16]

; TODO: 삭제해야 함
global setup_start
extern main

; ================================================
; GDT(Global Descripter Table)  
; 메모리 구역마다 권한과 크기를 정함으로써 
; 메모리를 보호하고, 32비트(4GB) 전체를 사용하기 위한 테이블
; ================================================
setup_start:
    cli             ; 모든 인터럽트를 중단
    ; GDT가 완성되지 않아 시스템 불안정
    ; Stack이 정의되지 않음
    ; 원자성 보장

    ; gdtr 메모리 주소로 가서 GDT를 읽고 GDT 레지스터에 저장
    ; size(2byte) / offset(4byte): gdt가 메모리 어디에 위치하는지
    lgdt [gdtr]


    ; cr0 = 0 : 16bit real mode
    ; cr0 = 1 ; 32bit protection mode
    mov eax, cr0
    or eax, 0x01
    mov cr0, eax

    ; dword(double word) = 32bit
    ; CS 레지스터를 0x08(코드 세그먼트)로 변경하고, 즉 GDT의 1번 규칙을 사용한다.
    ; 32비트 주소로 점프하면서, CPU 파이프라인에 남아있던 16비트 쓰레기 명령어들을 모두 비움.
    jmp dword 0x08:protected_mode_entry


; TODO: GDT Entry의 구조를 읽기 쉽게 리팩토링해야함
; ===========================================
; GDT entry 
;   Base (32bit)        : 세그먼트의 시작 주소
;   Limit (20bit)       : 세그먼트의 크기
;   Access Byte (8bit)  : 권한, 타입 등
;   Flags (4bit)        : 크기 단위, 모드 등
; ===========================================
gdt_start:
    ; 1. NULL desciptor:
    ; GDT의 시작은 8바이트의 0으로 시작해야 함.
    ; 잘못된 세그먼트 참조를 방지
    dd 0x00000000, 0x00000000

    ; 2. Code Segment Descriptor:
    ; Limit (0-15bit) = 0xFFFF
    ; Base (0-31bit) = 0x00000000 (메모리 시작점부터)
    ; Access Byte (0x9A): P=1, DPL=0, S=1, Type=Code (읽기/실행 가능)
    ; Flags (0xC): G=1 (4KB 단위), D=1 (32비트 모드)
    ; 0번지부터 4GB 전체를 아우르는 '실행 가능' 코드 영역 정의
    dd 0x0000FFFF, 0x00CF9A00

    ; 3. Data Segment Descriptor:
    ; Limit (0-15bit) = 0xFFFF
    ; Access Byte (0x92): P=1, DPL=0, S=1, Type=Data (읽기/쓰기 가능)
    ; Flags (0xC): G=1 (4KB 단위), D=1 (32비트 모드)
    ; 0번지부터 4GB 전체를 아우르는 '읽기/쓰기 가능' 데이터/스택 영역 정의
    dd 0x0000FFFF, 0x00CF9200
gdt_end:

gdtr:
    ; dw = 2byte / dd = 4byte
    dw gdt_end - gdt_start - 1  ; limit
    dd gdt_start                ; base address


[BITS 32]

protected_mode_entry:
    ; 세그먼트 레지스터들을 0x10(데이터 세그먼트), 즉 GDT의 2번 규칙을 적용하게 함
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; 스택 포인터와, 베이스 포인터의 위치를 0x90000으로 하고,
    ; 스택은 밑으로 내려가는 방향으로
    mov esp, 0x90000
    mov ebp, esp

    ; 커널을 부름
    call main

    ; 커널이 예기치 않게 종료 되었을 때 시스템을 정지 시킴
    jmp $