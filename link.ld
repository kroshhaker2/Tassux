OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    . = 0x00100000;

    .multiboot ALIGN(8) :
    {
        KEEP(*(.multiboot))
    }

    .text ALIGN(16) :
    {
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
    }

    .data ALIGN(16) :
    {
        *(.data)
        *(.data*)
    }

    .bss ALIGN(16) :
    {
        *(COMMON)
        *(.bss)
        *(.bss*)
    }
}