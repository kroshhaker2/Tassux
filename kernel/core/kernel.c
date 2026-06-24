#include "kernel.h"

#include "../console/command.h"
#include "../console/console.h"
#include "../include/stddef.h"
#include "../include/string.h"
#include "interrupts.h"

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

void kernel_main(unsigned int magic)
{
    console_init();
    //interrupts_init();

    for (;;)
    {
    }
}