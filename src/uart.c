#include "uart.h"
#include "timer.h"
#include "ringbuf.h"

#include <float.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <util/delay.h> // delay_ms

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
    int RXCIE; /* RX Complete Interrupt Enable */
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
ringbuffer uart_rxbuf[4];

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
    loop_until_bit_is_set(*(uart_regs[uart].UCSRA), uart_regs[uart].UDRE);
    *(uart_regs[uart].UDR) = c;
}

void uart_sendmsg(int uart, char *msg, int len) {
    if (len == -1) {
        // Try to get length using strlen if no length was specified
        len = strlen(msg);
    }

    if (uart == DBG_UART) {
        // Print timestamp first on DBG_UART
        char buf[255] = { '\0' };
        double uptime = (double)timer_get_uptime()/(double)1000.00;
        snprintf(buf, 255, "[%lf] ", uptime);
        for (int i = 0; i < strlen(buf); i++) {
            uart_putchar(uart, buf[i]);
        }
    }

    for (int i = 0; i < len; i++) {
        if (msg[i] == '\0') {
            // Break on null byte
            break;
        }

        uart_putchar(uart, msg[i]);
    }
}

void uart_clearbuf(int uart) {
    // Clears receive buffer of given UART
    *(uart_regs[uart].UDR);

    ringbuf_init(&uart_rxbuf[uart]);
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
    char c = '\0';

    int aritems = 0;
    int arsize = 0;

    while (!ringbuf_has_new_data(&uart_rxbuf[uart])) {
        // Wait for 1/4 the time of the buffer to fill
        _delay_ms(((BAUD/1000)*RINGBUF_MAX_SIZE)/4);
    }

    while (ringbuf_has_new_data(&uart_rxbuf[uart])) {
        // Get data from ring buffer
        c = ringbuf_read_char(&uart_rxbuf[uart]);

        // Ignore CRs
        if (c == '\r') {
            continue;
        }

        // Break on LF
        if (c == '\n') {
            break;
        }

        // Append current character to message buffer
        string_append(&msgbuf, c, &aritems, &arsize);
    }

    // Return null-terminated string
    string_append(&msgbuf, '\0', &aritems, &arsize);

#ifdef DEBUG
    // Print parsed message to DBG_UART in DEBUG mode
    if (uart == GSM_UART) {
        char buf[64] = { '\0' };
        snprintf(buf, 64, "Received '%s' via UART '%i'\r\n", msgbuf, uart);
        uart_sendmsg(DBG_UART, buf, 64);
    }
#endif
    return msgbuf;
}

void uart_init_portstruct() {
    // Initialise port structs
    uart_regs[UART0] = *(uart_registers[]){{&UBRR0L, &UBRR0H, &UCSR0A, &UCSR0B, &UCSR0C, &UDR0, UCSZ00, UCSZ01, UCSZ02, UDRE0, RXCIE0, RXEN0, TXEN0, RXC0, TXC0}};
    uart_regs[UART1] = *(uart_registers[]){{&UBRR1L, &UBRR1H, &UCSR1A, &UCSR1B, &UCSR1C, &UDR1, UCSZ10, UCSZ11, UCSZ12, UDRE1, RXCIE1, RXEN1, TXEN1, RXC1, TXC1}};
    uart_regs[UART2] = *(uart_registers[]){{&UBRR2L, &UBRR2H, &UCSR2A, &UCSR2B, &UCSR2C, &UDR2, UCSZ20, UCSZ21, UCSZ22, UDRE2, RXCIE2, RXEN2, TXEN2, RXC2, TXC2}};
    uart_regs[UART3] = *(uart_registers[]){{&UBRR3L, &UBRR3H, &UCSR3A, &UCSR3B, &UCSR3C, &UDR3, UCSZ30, UCSZ31, UCSZ32, UDRE3, RXCIE3, RXEN3, TXEN3, RXC3, TXC3}};
    uart_portstate.uart_regs_initialized = true;
}

void uart_buffer_rx(int uart) {
    char foo = *(uart_regs[uart].UDR);
    ringbuf_add_char(&uart_rxbuf[uart], foo);

#ifdef DEBUG
    char buf[128] = { '\0' };
    snprintf(buf, 128, "=============\r\nrpos: %lu\r\nwpos:%lu\r\nchar:%c\r\n", uart_rxbuf[uart].rpos, uart_rxbuf[uart].wpos, foo);
    uart_sendmsg(DBG_UART, buf, 128);
    uart_sendmsg(DBG_UART, "=============\r\n", -1);
#endif
}

ISR(USART0_RX_vect) {
    // Interrupt Service Routine for UART0
    uart_buffer_rx(UART0);
}

ISR(USART1_RX_vect) {
    // Interrupt Service Routine for UART1
    uart_buffer_rx(UART1);
}

ISR(USART2_RX_vect) {
    // Interrupt Service Routine for UART2
    uart_buffer_rx(UART2);
}

ISR(USART3_RX_vect) {
    // Interrupt Service Routine for UART3
    uart_buffer_rx(UART3);
}

void uart_init(int uart) {
    /* Initializes UART with configured
     * baud rate and 8-N-1
     */
    if (!uart_portstate.uart_regs_initialized) {
        // Initialize port struct and receive ring buffer only once
        uart_init_portstruct();

        // Initialise ring buffer
        ringbuf_init(&uart_rxbuf[uart]);
    }

    *(uart_regs[uart].UBRRL) = BAUDRATE&0xFF; /* Set bit rate to lower */
    *(uart_regs[uart].UBRRH) = (BAUDRATE>>8); /* and upper area */
    *(uart_regs[uart].UCSRA) &= ~(_BV(U2X0)); /* Disable 2X */

    *(uart_regs[uart].UCSRC) = _BV(uart_regs[uart].UCSZ1) | _BV(uart_regs[uart].UCSZ0); /* 8-bit data */
    *(uart_regs[uart].UCSRB) = _BV(uart_regs[uart].RXEN) | _BV(uart_regs[uart].TXEN) | _BV(uart_regs[uart].RXCIE); /* Enable RX, TX and Interrupt */

#ifdef DEBUG
    char buf[64] = { '\0' };
    snprintf(buf, 64, "uart_rxbuf[%i] addr:\t%p\r\n", uart, &uart_rxbuf[uart]);
    uart_sendmsg(DBG_UART, buf, 64);
#endif

    // Clear UDR and circular buffer
    uart_clearbuf(uart);
    ringbuf_init(&uart_rxbuf[uart]);

    // Enable interrupts
    sei();
}
