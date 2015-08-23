#include "config.h"
#include <avr/io.h>
#include <stdio.h>

int uart_putchar(char c, FILE *stream);
void uart_init(void);
