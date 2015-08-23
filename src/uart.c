#include "uart.h"

int uart_putchar(char c, FILE *stream) {
    if( c == '\n' )
        uart_putchar( '\r', stream );
    UDR0 = c;
    loop_until_bit_is_set(UCSR0A, TXC0);
    return 0;
}

static FILE uart_out = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void uart_init(void) {
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
