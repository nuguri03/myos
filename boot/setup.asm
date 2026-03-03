[BITS 16]

global setup_start
extern main

setup_start:
    cli

    lgdt [gdtr]

    mov eax, cr0
    or eax, 0x01
    mov cr0, eax

    jmp dword 0x08:protected_mode_entry

gdt_start:
    dd 0x00000000, 0x00000000

    dd 0x0000FFFF, 0x00CF9A00

    dd 0x0000FFFF, 0x00CF9200
gdt_end:

gdtr:
    dw gdt_end - gdt_start - 1
    dd gdt_start


[BITS 32]

protected_mode_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x90000
    mov ebp, esp

    call main

    jmp $