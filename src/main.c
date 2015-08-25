#include "uart.h"
#include "led.h"
#include "gsm.h"

int main (void) {
    // Initialize UART, enable transmission
    uart_init();
    printf("steep_beta v0.1\r\n");
    
    // Initialize LEDs
    led_init();
    
    // Initialize GSM Modem
    gsm_init();

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
