#pragma once

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

/* Регистры относительно базового адреса */

#define UART_DATA       0 // TX/RX
#define UART_IER        1 // Interrupt Enable
#define UART_DLL        0 // Divisor Latch Low
#define UART_DLM        1 // Divisor Latch High
#define UART_IIR        2 // Interrupt Identification
#define UART_FCR        2 // FIFO Control
#define UART_LCR        3 // Line Control
#define UART_MCR        4 // Modem Control
#define UART_LSR        5 // Line Status
#define UART_MSR        6 // Modem Status
#define UART_SCR        7 // Scratch

/* LCR bits */

#define UART_LCR_DLAB   0x80

#define UART_LCR_5BIT   0x00
#define UART_LCR_6BIT   0x01
#define UART_LCR_7BIT   0x02
#define UART_LCR_8BIT   0x03

#define UART_LCR_STOP2  0x04

#define UART_LCR_PARITY 0x08

/* FCR bits */

#define UART_FCR_ENABLE_FIFO  0x01
#define UART_FCR_CLEAR_RX     0x02
#define UART_FCR_CLEAR_TX     0x04

/* MCR bits */

#define UART_MCR_DTR    0x01
#define UART_MCR_RTS    0x02
#define UART_MCR_OUT1   0x04
#define UART_MCR_OUT2   0x08
#define UART_MCR_LOOP   0x10

/* LSR bits */

#define UART_LSR_DATA_READY    0x01
#define UART_LSR_OVERRUN_ERR   0x02
#define UART_LSR_PARITY_ERR    0x04
#define UART_LSR_FRAMING_ERR   0x08
#define UART_LSR_BREAK_INT     0x10
#define UART_LSR_THR_EMPTY     0x20
#define UART_LSR_TX_EMPTY      0x40
#define UART_LSR_FIFO_ERR      0x80

/* Скорости */

#define UART_BAUD_115200 115200
#define UART_BAUD_57600   57600
#define UART_BAUD_38400   38400
#define UART_BAUD_19200   19200
#define UART_BAUD_9600     9600

#include "../../include/stddef.h"
#include "../../include/stdint.h"

void serial_init(uint16_t port, uint32_t baud);
void serial_write_char(uint16_t port, char c);
void serial_write(uint16_t port, const char *str);
void serial_write_buf(uint16_t port, const char *buf, size_t len);