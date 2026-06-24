#include "serial.h"

#include "../../include/io.h"

void serial_init(uint16_t port, uint32_t baud)
{
    uint16_t divisor = 115200 / baud;

    outb(port + UART_IER, 0x00);

    outb(port + UART_LCR, UART_LCR_DLAB);

    outb(port + UART_DLL, divisor & 0xFF);
    outb(port + UART_DLM, (divisor >> 8) & 0xFF);

    outb(port + UART_LCR, UART_LCR_8BIT);

    outb(port + UART_FCR,
         UART_FCR_ENABLE_FIFO |
         UART_FCR_CLEAR_RX |
         UART_FCR_CLEAR_TX);

    outb(port + UART_MCR,
         UART_MCR_RTS |
         UART_MCR_DTR |
         UART_MCR_OUT2);
}

void serial_write_char(uint16_t port, char c)
{
    while (!(inb(port + UART_LSR) & UART_LSR_THR_EMPTY));
    outb(port + UART_DATA, c);
}

void serial_write(uint16_t port, const char *str)
{
    while (*str)
    {
        if (*str == '\n')
            serial_write_char(port, '\r');

        serial_write_char(port, *str++);
    }
}

void serial_write_buf(uint16_t port, const char *buf, size_t len)
{
    for (size_t i = 0; i < len; i++)
        serial_write_char(port, buf[i]);
}