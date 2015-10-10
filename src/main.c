#include "uart.h"
#include "gsm.h"
#include "io.h"
#include <util/delay.h>

void print_info() {
    printf("LED:\t\t%i\n", io_get_port_state(PORT_LED));
    printf("GSM_IGN:\t%i\n", io_get_port_state(PORT_GSM_IGN));
    printf("ALARM:\t\t%i\n\n", io_get_port_state(PORT_ALARM_INDICATOR));
}

int main (void) {
    // Initialize UART, enable transmission
    uart_init();
    printf("steep_beta v0.1\r\n");
    
    // Initialize GSM Modem
    gsm_init();

    while(1) {
        /* set pin 5 high to turn led on */
        printf("Turn LED on..\n");
        io_set_port_state(PORT_LED, IO_PORT_HIGH);
        print_info();
        _delay_ms(10000);

        /* set pin 5 low to turn led off */
        printf("Turn LED off..\n");
        io_set_port_state(PORT_LED, IO_PORT_LOW);
        print_info();
        _delay_ms(5000);
    }
}
