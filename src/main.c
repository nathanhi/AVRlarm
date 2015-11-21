#include "uart.h"
#include "gsm.h"
#include "io.h"
#include <util/delay.h>

#ifdef DEBUG
void print_info() {
    printf("LED:\t\t%i\n", io_get_port_state(PORT_LED));
    printf("GSM_IGN:\t%i\n", io_get_port_state(PORT_GSM_IGN));
    printf("ALARM:\t\t%i\n\n", io_get_port_state(PORT_ALARM_INDICATOR));
}
#endif

int main (void) {
    // Initialize UART, enable transmission
    uart_init();

    // Initialize GSM Modem
    gsm_init();

#ifdef INITIAL_WAIT
    // Wait for specified initial wait time
    _delay_ms(INITIAL_WAIT);
#endif

    // Send initialization SMS
    gsm_send_sms("steep_beta has been initialized, status: AOK", TGT_NUM);

    while(1) {
        // Power on status LED
        io_set_port_state(PORT_LED, IO_PORT_HIGH);

#ifdef DEBUG
        // Print debug output
        print_info();
#endif

        // Pull alarm IO port to low
        io_set_port_state(PORT_ALARM_INDICATOR, IO_PORT_LOW);

        // Check for IO_PORT_HIGH
        if (io_get_port_state(PORT_ALARM_INDICATOR) == IO_PORT_HIGH)
            gsm_send_sms("ALAAARM!!11", TGT_NUM);

        // Wait first half of scan interval
        _delay_ms(SCAN_INTERVAL);
        
        // Disable LED
        io_set_port_state(PORT_LED, IO_PORT_LOW);
        
        // Wait the other half
        _delay_ms(SCAN_INTERVAL);
    }
}
