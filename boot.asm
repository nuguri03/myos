[ORG 0x7c00]
[BITS 16]

start:
    mov si, msg
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
    
msg db 'Hello BootLoader!', 0

times 510-($-$$) db 0
dw 0xAA55