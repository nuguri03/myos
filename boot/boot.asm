; KERNEL SETTINGS
%define BOOTLOADER_ADDR 0x7c00
%define KERNEL_SEGMENT  0x0800 
%define KERNEL_OFFSET   0x0000

; BIOS가 이 코드를 BOOTLOADER_ADDR(0x7c00)에 적재할 것이므로,
; 컴파일러에게 이 곳을 기준으로 주소를 계산하라고 알림
[ORG BOOTLOADER_ADDR]
[BITS 16]             ; 아래의 코드는 16비트 체제이다.

; BIOS CONSTANTS    
%define FIRST_HDD       0x80
%define BIOS_READ_DISK  0x02
%define BIOS_DISK_INT   0x13 
%define BIOS_TTY_OUTPUT 0x0e
%define BIOS_VIDEO_INT  0x10

start:
    ; 물리 주소 = (es * 16) + bx
    ; es(extra segment):bx(base register) = 0x0800:0000 -> 0x8000
    ; 디스크에서 읽은 내용을 0x8000 부터 차례로 저장해라
    mov ax, KERNEL_SEGMENT
    mov es, ax
    mov bx, KERNEL_OFFSET

    ; 하드디스크에 저장된 데이터를 메모리로 복사
    mov ah, BIOS_READ_DISK      ; 디스크 섹터를 읽어라는 명령
    mov al, KERNEL_SECTORS      ; KERNEL_SECTORS = 커널의 크기(Makefile에 정의되어 있음)
    mov ch, 0                   ; 디스크의 0번째 트랙부터
    mov cl, 2                   ; 2번째 섹터부터(1번 섹터는 MBR으로 pass)
    mov dh, 0                   ; 0번째 헤드부터
    mov dl, FIRST_HDD           ; 0x80(첫번째 HDD)
    ; 정리하자면 첫번째 HDD의 0번 실린더의 2번째 섹터의 0번째 헤드부터 KERNEL_SECTORS 크기만큼 디스크를 읽어라


    int BIOS_DISK_INT           ; BIOS 디스크 서비스 호출(디스크에서 읽어라 ES:BX 메모리에 적재)
    jc disk_error               ; 플래그 레지스터 중 Carry Flag가 1(Error) 이면 disk_error로 점프

    jmp KERNEL_SEGMENT:KERNEL_OFFSET ; 0x8000번지로 점프

disk_error:
    mov si, error_msg
    call print
    jmp $

print:
    mov ah, BIOS_TTY_OUTPUT     ; BIOS Video Service의 `Teletype Output` (한글자 출력) 기능 번호
.loop:
    lodsb
    cmp al, 0
    je .end
    int BIOS_VIDEO_INT          ; 화면 출력을 담당하는 BIOS 인터럽트 번호
    jmp .loop
.end:
    ret

error_msg db 'Disk Read Failed', 0

; $: 현재 위치, $$: 현재 섹션의 시작 위치
times 510-($-$$) db 0           ; 코드가 쓰이고 남은 공간을 0으로 채워라
dw 0xAA55                       ; 부팅이 가능한 것임을 밝히는 시그니처(x86은 리틀 엔디안 방식을 사용함,즉 0x55AA를 읽음)