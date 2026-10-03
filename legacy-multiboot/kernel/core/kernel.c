#include "kernel.h"

#include "../console/command.h"
#include "../console/console.h"
#include "../include/stddef.h"
#include "../include/string.h"
#include "interrupts.h"
#include "../drivers/serial/serial.h"

volatile unsigned int timer_ticks = 0;

void pit_handler() { timer_ticks++; }

void sleep_ms(unsigned int ms)
{
    unsigned int start = timer_ticks;
    while (timer_ticks - start < ms)
        ;
}

void panic(const char *msg)
{
    print_colored("KERNEL PANIC: ", LIGHT_RED | (BLACK << 4));
    print(msg);
    print("\nSYSTEM HALTED.\n");
    print("Reboot the system to continue.\n");
    asm volatile("cli; hlt");
}

void kernel_main(uint32_t magic, void *mb_info)
{
    volatile uint16_t *vga = (uint16_t *)0xB8000;

    vga[0] = 0x0F4F;
    vga[1] = 0x0F4B;

    for (;;);
}