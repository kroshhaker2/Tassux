[BITS 32]

section .multiboot
align 8
header_start:
    dd 0xe85250d6
    dd 0
    dd header_end - header_start
    dd -(0xe85250d6 + (header_end - header_start))
    dw 0
    dw 0
    dd 8
header_end:

section .text
global _start

_start:
    mov dword [0xB8000], 0x0F4B0F4F ; "OK"
.loop:
    hlt
    jmp .loop