#ifndef UART_H
#define UART_H

#include "config.h"
#include <stdio.h>

#define UART0 0
#define UART1 1
#define UART2 2
#define UART3 3

#define GSM_UART UART1
#define DBG_UART UART0

void uart_putchar(int uart, char c);
void uart_sendmsg(int uart, char *msg);
char *uart_getmsg(int uart);
void uart_init(int uart);
void uart_clearbuf(int uart);

#endif /* UART_H */
