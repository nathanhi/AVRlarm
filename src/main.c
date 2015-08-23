// 16MHz + 9600 Baud
// Possible bit rates:
// http://bit.ly/1ND0wxf
#define F_CPU 16000000
#define BAUD 9600
#define BAUDRATE (((F_CPU)/(BAUD*16UL))-1)

#define BLINK_DELAY_MS 1000

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

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

int main (void) {
    // Initialize UART, enable transmission
    uart_init();

    /* set pin 5 of PORTB for output*/
    DDRB |= _BV(DDB5);
    int i = 0;

    while(1) {
        i++;
        printf("%i\r\n", i);
        
        /* set pin 5 high to turn led on */
        PORTB |= _BV(PORTB5);
        _delay_ms(BLINK_DELAY_MS);

        /* set pin 5 low to turn led off */
        PORTB &= ~_BV(PORTB5);
        _delay_ms(BLINK_DELAY_MS);
    }
}
