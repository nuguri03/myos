[ORG 0x7c00]
[BITS 16]

start:
    mov ax, 0x0800
    mov es, ax
    mov bx, 0x0000

    mov ah, 0x02
    mov al, KERNEL_SECTORS      ; KERNEL_SECTOR = 커널의 크기
    mov ch, 0
    mov cl, 2                   ; 2번째 섹터부터
    mov dh, 0                   ; 0번째 헤드부터
    mov dl, 0x80                ; 0x80(HDD)

    int 0x13
    jc disk_error

    jmp 0x0800:0000

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

times 510-($-$$) db 0
dw 0xAA55