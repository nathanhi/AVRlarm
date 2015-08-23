#include "uart.h"
#include <util/delay.h>

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
        _delay_ms(1000);

        /* set pin 5 low to turn led off */
        PORTB &= ~_BV(PORTB5);
        _delay_ms(1000);
    }
}
