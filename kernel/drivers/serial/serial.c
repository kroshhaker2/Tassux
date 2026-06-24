#include "serial.h"

#include "../../include/io.h"

void serial_write(char c)
{
    while (!(inb(0x3F8 + 5) & 0x20));
    outb(0x3F8, c);
}