[BITS 32]

section .text
global context_switch

context_switch:
    push ebp
    push ebx
    push esi
    push edi

    mov eax, [esp + 20] ; 현재 스레드의 ESP를 저장할 변수 주소 (old_esp)
    mov [eax], esp      ; 그 변수에 현재 ESP 저장
    mov ebx, [esp + 24] ; 다음 스레드의 저장된 ESP 값 (new_esp)
    mov esp, ebx        ; 다음 스레드의 스택으로 전환
    pop edi

    pop esi
    pop ebx
    pop ebp
    ret