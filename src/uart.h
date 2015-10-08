#include "config.h"
#include <stdio.h>

void uart_putchar(char c, FILE *stream);
char *uart_getmsg();
void uart_init(void);
