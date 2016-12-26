#include "uart.h"
#include "gsm.h"
#include "io.h"
#include "timer.h"
#include <util/delay.h> // delay_ms
#include <string.h> // memset

#ifdef DEBUG
void print_info() {
    char buf[32] = { '\0' };
    snprintf(buf, 32, "LED:\t\t%i\n", io_get_port_state(PORT_LED));
    uart_sendmsg(UART0, buf, 32);
    memset(&buf[0], '\0', 32);
    snprintf(buf, 32, "GSM_IGN:\t%i\n", io_get_port_state(PORT_GSM_IGN));
    uart_sendmsg(UART0, buf, 32);
    memset(&buf[0], '\0', 32);
    snprintf(buf, 32, "ALARM:\t\t%i\n\n", io_get_port_state(PORT_ALARM_INDICATOR));
    uart_sendmsg(UART0, buf, 32);
}
#endif

int main (void) {
    // Initialise uptime timer
    timer_init();

    // Initialize UART, enable transmission
    uart_init(DBG_UART);

    // Initialize GSM Modem
    gsm_init();

#ifdef INITIAL_WAIT
    // Wait for specified initial wait time
    _delay_ms(INITIAL_WAIT);
#endif

    // Send initialization SMS
    gsm_send_sms(ARMMSG, INFO_NUMS);

    uart_sendmsg(DBG_UART, "[ALARM]: Beginning alarm loop!\n", -1);

    while(1) {
        // Power on status LED
        io_set_port_state(PORT_LED, IO_PORT_HIGH);

#ifdef DEBUG
        // Print debug output
        print_info();
#endif

        // Pull alarm IO port to low
        io_set_port_state(PORT_ALARM_INDICATOR, IO_PORT_LOW);

        // Check for IO_PORT_LOW
        // LOW == intrusion
        if (io_get_port_state(PORT_ALARM_INDICATOR) == IO_PORT_LOW) {
            uart_sendmsg(DBG_UART, "[ALARM]: Intrusion detected!\n", -1);
            // Disable all LEDs to avoid detection
            io_set_port_state(PORT_LED, IO_PORT_LOW);

            // Wait for configured time
            _delay_ms(ALARM_WAIT);

            // Send SMS
            gsm_send_sms(ALARMMSG, TGT_NUMS);

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
        _delay_ms(SCAN_INTERVAL/2);

        // Disable LED
        io_set_port_state(PORT_LED, IO_PORT_LOW);

        // Wait the other half
        _delay_ms(SCAN_INTERVAL/2);
    }
}
