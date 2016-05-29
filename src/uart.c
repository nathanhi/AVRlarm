#include "uart.h"
#include <string.h>
#include <stdlib.h>

typedef struct uart_registers {
    volatile uint8_t *UBRRL; /* Bit rate lower register (8-Bit) */
    volatile uint8_t *UBRRH; /* Bit rate upper register (8-Bit) */
    volatile uint8_t *UCSRA; /* 2X */
    volatile uint8_t *UCSRB; /* RXC Interrupt + RX/TX Ports */
    volatile uint8_t *UCSRC; /* Parity + Stopbit */
    volatile uint8_t *UDR;   /* UART data register */
    int UCSZ0; /* Character size */
    int UCSZ1; /* Character size */
    int UCSZ2; /* Character size */
    int UDRE; /* Data Register Empty bit */
    int RXEN; /* RX enable bit */
    int TXEN; /* TX enable bit */
    int RXC; /* RX complete bit */
    int TXC; /* TX complete bit */
} uart_registers;

typedef struct uart_ports {
    bool uart1_initialized;
    bool uart2_initialized;
    bool uart3_initialized;
    bool uart_regs_initialized;
} uart_ports;

uart_registers uart_regs[4];
uart_ports uart_portstate = {false};

void string_append(char **str, char c, int *aritems, int *arsize) {
    /* Appends c to str1. */
    (*arsize) += sizeof(char);
    void *_artmp = realloc(*str, (*arsize));
    (*str) = (char*)_artmp;
    (*str)[(*aritems)] = c;
    (*aritems)++;
}

void uart_putchar(int uart, char c) {
    /* Writes a character to UART,
     * automatic CRLF completion
     */
    if (c == '\n')
        uart_putchar(uart, '\r');

    loop_until_bit_is_set(*(uart_regs[uart].UCSRA), uart_regs[uart].UDRE);
    *(uart_regs[uart].UDR) = c;
}

void uart_sendmsg(int uart, char *msg) {
    for (int i = 0; i < strlen(msg); i++) {
        uart_putchar(uart, msg[i]);
    }
}

void uart_clearbuf(int uart) {
    // Clears receive buffer of given UART
    *(uart_regs[uart].UDR);
}

char uart_getchar(int uart) {
    /* Receives one character from UART */
    char tmp;
    tmp = 0;
    if (*(uart_regs[uart].UCSRA) & (1<<uart_regs[uart].RXC)) {
        tmp = *(uart_regs[uart].UDR);
        while (!(*(uart_regs[uart].UCSRA) & (1<<uart_regs[uart].UDRE)));
    }
    return tmp;
}

char *uart_getmsg(int uart) {
    /* Receives complete messages
     * until CR or LF from given UART
     */
    char *msgbuf = NULL;
    char lastchar;

    int aritems = 0;
    int arsize = 0;

    while (true) {
        // Loop to get all chars
        lastchar = uart_getchar(uart);

        // Continue until buffer is filled
        if (lastchar == 0)
            continue;

        // The modem clears its output with \r
        if (lastchar == '\r') {
            uart_clearbuf(uart);
            continue;
        }

        // Abort loop if LF has been received
        if (lastchar == '\n')
            break;
        
        // Append current character to message buffer
        string_append(&msgbuf, lastchar, &aritems, &arsize);
    }

    // Clear UART message buffer afterwards (ignore everything after \n)
    uart_clearbuf(uart);

    // Return null-terminated string
    string_append(&msgbuf, '\0', &aritems, &arsize);
    return msgbuf;
}

void uart_init_portstruct() {
    uart_regs[UART0] = *(uart_registers[]){{&UBRR0L, &UBRR0H, &UCSR0A, &UCSR0B, &UCSR0C, &UDR0, UCSZ00, UCSZ01, UCSZ02, UDRE0, RXEN0, TXEN0, RXC0, TXC0}};
    uart_regs[UART1] = *(uart_registers[]){{&UBRR1L, &UBRR1H, &UCSR1A, &UCSR1B, &UCSR1C, &UDR1, UCSZ10, UCSZ11, UCSZ12, UDRE1, RXEN1, TXEN1, RXC1, TXC1}};
    uart_regs[UART2] = *(uart_registers[]){{&UBRR2L, &UBRR2H, &UCSR2A, &UCSR2B, &UCSR2C, &UDR2, UCSZ20, UCSZ21, UCSZ22, UDRE2, RXEN2, TXEN2, RXC2, TXC2}};
    uart_regs[UART3] = *(uart_registers[]){{&UBRR3L, &UBRR3H, &UCSR3A, &UCSR3B, &UCSR3C, &UDR3, UCSZ30, UCSZ31, UCSZ32, UDRE3, RXEN3, TXEN3, RXC3, TXC3}};
    uart_portstate.uart_regs_initialized = true;
}

void uart_init(int uart) {
    /* Initializes UART with configured
     * baud rate and 8-N-1
     */
    if (!uart_portstate.uart_regs_initialized) {
        // Initialize port struct only once
        uart_init_portstruct();
    }

    *(uart_regs[uart].UBRRL) = BAUDRATE&0xFF; /* Set bit rate to lower */
    *(uart_regs[uart].UBRRH) = (BAUDRATE>>8); /* and upper area */
    *(uart_regs[uart].UCSRA) &= ~(_BV(U2X0)); /* Disable 2X */

    *(uart_regs[uart].UCSRC) = _BV(uart_regs[uart].UCSZ1) | _BV(uart_regs[uart].UCSZ0); /* 8-bit data */
    *(uart_regs[uart].UCSRB) = _BV(uart_regs[uart].RXEN) | _BV(uart_regs[uart].TXEN);   /* Enable RX and TX */
}
