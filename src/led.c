#include "led.h"
#include <util/delay.h>

void led_init() {
    // Set pin 5 of PORTB for output
    DDRB |= _BV(DDB5);

    // Light up LED for configured timeout INITIAL_WAIT
    PORTB |= _BV(PORTB5);
    _delay_ms(INITIAL_WAIT);
    PORTB &= ~_BV(PORTB5);
}
