#include "interrupts.h"

#include "../include/io.h"
#include "../include/string.h"

extern void keyboard_handler(); // ASM stub
extern void load_idt(unsigned long *);

static struct idt_entry idt[IDT_SIZE];

static void idt_set_gate(uint8_t n, uint32_t handler)
{
    idt[n].offset_low = handler & 0xFFFF;
    // В Multiboot загрузчике обычно используется плоская модель памяти:
    // 0x08 = код ядра, 0x10 = данные ядра
    // Но некоторые загрузчики могут использовать другие значения
    idt[n].selector = 0x08; // kernel code segment (может быть 0x10 в некоторых случаях)
    idt[n].zero = 0;
    idt[n].type_attr = 0x8E; // present, ring0, interrupt gate
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

static void pic_remap(void)
{
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20); // master offset 32
    outb(0xA1, 0x28); // slave offset 40

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0xff);
    outb(0xA1, 0xff);
}

void interrupts_init(void)
{
    struct idt_ptr idt_ptr;

    memset(idt, 0, sizeof(idt));

    pic_remap();

    idt_set_gate(0x21, (unsigned long)keyboard_handler); // IRQ1

    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (unsigned long)idt;

    load_idt((unsigned long *)&idt_ptr);

    outb(0x21, 0b11111101);

    asm volatile("sti");
}
