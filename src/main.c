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
    gsm_send_sms(ARMMSG, TGT_NUM);

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
        if (io_get_port_state(PORT_ALARM_INDICATOR) == IO_PORT_HIGH) {
            gsm_send_sms(ALARMMSG, TGT_NUM);

#ifdef ALARM_RESUME
            /* If alarm system is allowed to resume after
             * an alarm, wait for specified time and continue
             */
            _delay_ms(ALARM_RESUME);
#else
            // If alarm system has to stay in error state, do so
            while (1) {
                io_set_port_state(PORT_LED, IO_PORT_LOW);
                _delay_ms(100);
                io_set_port_state(PORT_LED, IO_PORT_HIGH);
                _delay_ms(100);
            }
#endif
    }

        // Wait first half of scan interval
        _delay_ms(SCAN_INTERVAL);
        
        // Disable LED
        io_set_port_state(PORT_LED, IO_PORT_LOW);
        
        // Wait the other half
        _delay_ms(SCAN_INTERVAL);
    }
}
