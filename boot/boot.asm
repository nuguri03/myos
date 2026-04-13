[ORG 0x7c00]    ; BIOS가 이 코드를 0x7c00에 적재할 것이므로, 컴파일러에게 이 곳을 기준으로 주소를 계산하라고 알림
[BITS 16]       ; 아래의 코드는 16비트 체제이다.

; TODO: magic numbers 리팩토링
; ============================================================================
; magic numbers & settings
; 0x0800        = 세그먼트 주소 (커널/setup.asm이 로드될 위치) 
; 0x02          = BIOS Disk Service의 'Read Sectors` 기능 번호
; 0x80          = 첫 번째 HDD를 의미하는 번호
; 0x0800:0000   = 로드가 완료된 후 점프할 주소
; 0x0e          = BIOS Video Service의 `Teletype Output` (한글자 출력) 기능 번호
; 0x10          = 화면 출력을 담당하는 BIOS 인터럽트 번호
; ============================================================================


start:
    ; 물리 주소 = (es * 16) + bx
    ; es(extra segment):bx(base register) = 0x0800:0000 -> 0x8000
    ; 디스크에서 읽은 내용을 0x8000 부터 차례로 저장해라
    mov ax, 0x0800
    mov es, ax
    mov bx, 0x0000

    ; 하드디스크에 저장된 데이터를 메모리로 복사
    mov ah, 0x02                ; 디스크 섹터를 읽어라는 명령
    mov al, KERNEL_SECTORS      ; KERNEL_SECTORS = 커널의 크기(Makefile에 정의되어 있음)
    mov ch, 0                   ; 디스크의 0번째 트랙부터
    mov cl, 2                   ; 2번째 섹터부터(1번 섹터는 MBR으로 pass)
    mov dh, 0                   ; 0번째 헤드부터
    mov dl, 0x80                ; 0x80(첫번째 HDD)
    ; 정리하자면 첫번째 HDD의 0번 실린더의 2번째 섹터의 0번째 헤드부터 KERNEL_SECTORS 크기만큼 디스크를 읽어라


    int 0x13                    ; BIOS 디스크 서비스 호출(디스크에서 읽어라 ES:BX 메모리에 적재)
    jc disk_error               ; 플래그 레지스터 중 Carry Flag가 1(Error) 이면 disk_error로 점프

    jmp 0x0800:0000             ; 0x8000번지로 점프

disk_error:
    mov si, err_msg
    call print
    jmp $

print:
    mov ah, 0x0e
.loop:
    lodsb
    cmp al, 0
    je .end
    int 0x10
    jmp .loop
.end:
    ret

msg db 'Successfully load kernel', 0
err_msg db 'Disk Read Failed', 0

; $: 현재 위치, $$: 현재 섹션의 시작 위치
times 510-($-$$) db 0           ; 코드가 쓰이고 남은 공간을 0으로 채워라
dw 0xAA55                       ; 부팅이 가능한 것임을 밝히는 시그니처(x86은 리틀 엔디안 방식을 사용함,즉 0x55AA를 읽음)