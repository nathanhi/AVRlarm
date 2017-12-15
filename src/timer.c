#include "timer.h"
#include "power.h"

#include <avr/interrupt.h>
#include <avr/wdt.h> // wdt_enable
#include <util/atomic.h>
#include <stdint.h>
#include <stdbool.h>

#define CTC_MATCH_OVERFLOW ((F_CPU/1000UL)/8UL)
volatile uint64_t timer1_ms;
bool timer_power_configured = false;

ISR(TIMER1_COMPA_vect) {
    timer1_ms++;

    // Reset on overflow to avoid random memory writes.
    if (timer1_ms == UINT64_MAX)
        wdt_enable(WDTO_15MS);
}

uint64_t timer_get_uptime() {
    /* Returns uptime since start of timer_init() */
    uint64_t ms_return;

    // Do not disrupt
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        ms_return = timer1_ms;
    }

    return ms_return;
}

powermgmt_callback timer_wakeup() {
    // Powermanagement wakeup callback, enable timer again
    PRR0 &= ~_BV(PRTIM1);
}

powermgmt_callback timer_sleep() {
    // Powermanagement sleep preparation callback, disable timer during sleep
    PRR0 |= _BV(PRTIM1);
}

void timer_configure_powermgmt() {
    // Configure power management if not already done
    if (timer_power_configured) {
        return;
    }
    timer_power_configured = true;

    // Register callbacks
    power_register_wakeup_callback(&timer_wakeup);
    power_register_sleep_callback(&timer_sleep);
}

void timer_init() {
    /* Initialises timer interrupt */
    TCCR1B |= (1 << WGM12) | (1 << CS11);

    // Trigger interrupt on CTC_MATCH_OVERFLOW
    OCR1AH = (CTC_MATCH_OVERFLOW >> 8);
    OCR1AL = (CTC_MATCH_OVERFLOW & 0xFF);

    /* Configure Timer Interrupt Mask register
     * to trigger interrupt on timer overflow
     */
    TIMSK1 |= _BV(OCIE1A);

    sei();

    // Register power management functions
    timer_configure_powermgmt();
}
