#include "uart.h"
#include "gsm.h"
#include "io.h"
#include <util/delay.h>

int main (void) {
    // Initialize UART, enable transmission
    uart_init();
    printf("steep_beta v0.1\r\n");
    
    // Initialize GSM Modem
    gsm_init();

    int i = 0;
    while(1) {
        i++;
        printf("%i\r\n", i);
        
        /* set pin 5 high to turn led on */
        io_set_port_state(PORT_LED, IO_PORT_HIGH);
        _delay_ms(1000);

        /* set pin 5 low to turn led off */
        io_set_port_state(PORT_LED, IO_PORT_LOW);
        _delay_ms(1000);
    }
}
