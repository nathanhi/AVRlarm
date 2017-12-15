#include "version.h"
#include "uart.h"
#include "gsm.h"
#include "io.h"
#include "timer.h"
#include "power.h"

#include <util/delay.h> // delay_ms
#include <string.h> // memset
#include <avr/interrupt.h> // sei
#include <avr/wdt.h> // wdt_enable()

#ifdef DEBUG
void print_info() {
    char buf[32] = { '\0' };
    snprintf(buf, 32, "[DBG] LED:\t\t\t%i\r\n", io_get_port_state(PORT_LED));
    uart_sendmsg(UART0, buf, 32);
    memset(&buf[0], '\0', 32);
    snprintf(buf, 32, "[DBG] GSM_IGN:\t\t%i\r\n", io_get_port_state(PORT_GSM_IGN));
    uart_sendmsg(UART0, buf, 32);
    memset(&buf[0], '\0', 32);
    snprintf(buf, 32, "[DBG] ALARM:\t\t\t%i\r\n", io_get_port_state(PORT_ALARM_INDICATOR));
    uart_sendmsg(UART0, buf, 32);
}

int freeMem() {
    extern int __heap_start, *__brkval;
    int v;
    int i = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    return i;
}
#endif

int main() {
    // Initialise uptime timer
    timer_init();

    // Initialize UART, enable transmission
    uart_init(DBG_UART);

    uart_sendmsg(DBG_UART, "steep beta v" STEEP_BETA_VERSION " (" STEEP_BETA_COMMIT "; " STEEP_BETA_BUILDDATE ")\r\n\r\n", -1);

#ifdef DEBUG
    char buf[64] = { '\0' };
    snprintf(buf, 64, "[DBG] SRAM free memory:\t%i bytes\r\n", freeMem());
    uart_sendmsg(DBG_UART, buf, 64);
    print_info();
    uart_putchar(DBG_UART, '\r');
    uart_putchar(DBG_UART, '\n');
#endif

    // Initialize GSM Modem
    gsm_init();

    // Set PORT_ALARM_INDICATOR as input
    io_port_init(PORT_ALARM_INDICATOR, IO_PORT_IN, true);
    
    // Wait until PORT_ALARM_INDICATOR is LOW (i.e. doors closed)
    while(io_get_port_state(PORT_ALARM_INDICATOR) != IO_PORT_LOW) {
        _delay_ms(500);
    }

    // Send initialisation SMS
    uart_sendmsg(DBG_UART, "Sending initialisation SMS..\r\n", -1);
    gsm_send_sms(ARMMSG, INFO_NUMS);

    // Power on status LED
    io_set_port_state(PORT_LED, IO_PORT_HIGH);

    // Enable interrupt for alarm indicator port, if pulled HIGH
    PCMSK0 |= _BV(PORT_ALARM_INDICATOR);
    PCICR |= _BV(PCIE0);
    EICRA |= _BV(ISC01) | _BV(ISC00);
    sei();

    // Initialise power management
    power_init();

    uart_sendmsg(DBG_UART, "[ALARM] Beginning alarm loop!\r\n", -1);

    while(1) {
        // Enter sleep mode
        power_sleep();

        // Woke up by interrupt; Disable all LEDs to avoid detection
        io_set_port_state(PORT_LED, IO_PORT_LOW);

        // Check port state after interrupt
        if (io_get_port_state(PORT_ALARM_INDICATOR) == IO_PORT_LOW) {
            // False alarm, enable LED and go to sleep again
            io_set_port_state(PORT_LED, IO_PORT_HIGH);
            continue;
        }

        // Port is HIGH, intrusion detected; wake up peripherals
        power_wakeup();

        // Print to console
        uart_sendmsg(DBG_UART, "[ALARM] Intrusion detected!\r\n", -1);

        // Wait for configured time
        _delay_ms(ALARM_WAIT);

        // Send SMS
        gsm_send_sms(ALARMMSG, TGT_NUMS);

#ifdef ALARM_RESUME
        /* If alarm system is allowed to resume after
         * an alarm, wait for specified time and continue
         */
        _delay_ms(ALARM_RESUME);
        io_set_port_state(PORT_LED, IO_PORT_HIGH);
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

    // Reset if while loop is somehow exited
    wdt_enable(WDTO_15MS);
}

// Handle interrupt
ISR(PCINT0_vect) { }
