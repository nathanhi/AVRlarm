#include "uart.h"
#include <string.h>
#include <stdlib.h>

void string_append(char **str, char c, int *aritems, int *arsize) {
    /* Appends c to str1. */
    (*arsize) += sizeof(char);
    void *_artmp = realloc(*str, (*arsize));
    (*str) = (char*)_artmp;
    (*str)[(*aritems)] = c;
    (*aritems)++;
}

void uart_putchar(char c, FILE *stream) {
    /* Writes a character to UART,
     * automatic CRLF completion */
    if (c == '\n')
        uart_putchar('\r', stream);
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
}

char uart_getchar() {
    /* Receives one character from UART */
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}

char *uart_getmsg() {
    /* Receives complete messages
     * until CR or LF from UART */
    char *msgbuf = NULL;
    char lastchar;

    int aritems = 0;
    int arsize = 0;

    while (true) {
        // Loop to get all chars
        lastchar = uart_getchar();

        // Append current character to message buffer
        string_append(&msgbuf, lastchar, &aritems, &arsize);

        if (lastchar == '\n') {
            // Abort loop if LF has been received
            break;
        } else if (lastchar == '\r') {
            // Append LF to received CR
            string_append(&msgbuf, '\n', &aritems, &arsize);
            break;
        }
    }
    // Return null-terminated string
    string_append(&msgbuf, '\0', &aritems, &arsize);
    return msgbuf;
}

static FILE uart_out = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void uart_init(void) {
    /* Initializes UART with configured
     * baud rate and 8-N-1 */
    UBRR0L = BAUDRATE&0xFF;
    UBRR0H = (BAUDRATE>>8);

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
    stdout = &uart_out;
}
