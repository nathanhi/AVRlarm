#include "timer.h"
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdint.h>

#define CTC_MATCH_OVERFLOW ((F_CPU/1000UL)/8UL)
volatile uint64_t timer1_ms;

ISR(TIMER1_COMPA_vect) {
    timer1_ms++;

    // Reset on overflow to avoid random memory writes.
    if (timer1_ms == UINT64_MAX)
        asm("JMP 0");
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

void timer_init() {
    /* Initialises timer interrupt */
    TCCR1B |= (1 << WGM12) | (1 << CS11);

    // Trigger interrupt on CTC_MATCH_OVERFLOW
    OCR1AH = (CTC_MATCH_OVERFLOW >> 8);
    OCR1AL = (CTC_MATCH_OVERFLOW & 0xFF);

    /* Configure Timer Interrupt Mask register
     * to trigger interrupt on timer overflow
     */
    TIMSK1 |= (1 << OCIE1A);

    sei();
}
