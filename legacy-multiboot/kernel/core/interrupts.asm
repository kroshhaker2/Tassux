[BITS 32]

global keyboard_handler
global load_idt
extern keyboard_irq

load_idt:
    mov edx, [esp + 4]
    lidt [edx]
    ret


keyboard_handler:
    pusha
    call keyboard_irq

    mov al, 0x20
    out 0x20, al      ; EOI

    popa
    iret
