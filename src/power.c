#include "power.h"
#include "uart.h" // Debugging output

#include <avr/io.h> // PRR, SMCR
#include <avr/wdt.h> // wdt_{dis,en}able

#define POWER_MAX_CALLBACKS  8

int *POWER_INIT_CALLBACKS[POWER_MAX_CALLBACKS];
int POWER_NUM_INIT_CALLBACKS = 0;
int *POWER_SLEEP_CALLBACKS[POWER_MAX_CALLBACKS];
int POWER_NUM_SLEEP_CALLBACKS = 0;
int *POWER_WAKEUP_CALLBACKS[POWER_MAX_CALLBACKS];
int POWER_NUM_WAKEUP_CALLBACKS = 0;

void add_callback_to_array(int *(*callback_array)[],
                           int *num_callbacks,
                           powermgmt_callback *c) {
    // Register given callback to given array
#ifdef DEBUG
    char buf[64];
    snprintf(buf, 64, "[DBG] Registering power callback with address %p\r\n", c);
    uart_sendmsg(DBG_UART, buf, 64);
#endif
    if (*num_callbacks == POWER_MAX_CALLBACKS) {
#ifdef DEBUG
        uart_sendmsg(DBG_UART, "[DBG] Attempting to register more callbacks than allowed, ignoring request.\r\n", -1);
#endif
        return;
    }

    // Add callback to array
    (*callback_array)[(*num_callbacks)] = c;
    (*num_callbacks)++;
}

void call_functions_from_pointer(int *(*callback_array)[],
                                 int *num_callbacks) {
    typedef powermgmt_callback callback_func(void);

    for (int i=0; i<(*num_callbacks); i++) {
#ifdef DEBUG
        char buf[64];
        snprintf(buf, 64, "[DBG] Executing callback at address %p\r\n", (*callback_array)[i]);
        uart_sendmsg(DBG_UART, buf, 64);
#endif
        callback_func* f = (void*)((*callback_array)[i]);
        f();
    }
}

void power_register_init_callback(powermgmt_callback *c) {
    // Register given callback for power management initialisation
    add_callback_to_array(&POWER_INIT_CALLBACKS, &POWER_NUM_INIT_CALLBACKS, c);
}

void power_register_sleep_callback(powermgmt_callback *c) {
    // Register given callback for sleep preparation (i.e. coordinated hardware shutdown)
    add_callback_to_array(&POWER_SLEEP_CALLBACKS, &POWER_NUM_SLEEP_CALLBACKS, c);
}

void power_register_wakeup_callback(powermgmt_callback *c) {
    // Register given callback for wakeup preparation (i.e. restoring parameters)
    add_callback_to_array(&POWER_WAKEUP_CALLBACKS, &POWER_NUM_WAKEUP_CALLBACKS, c);
}

void power_init() {
    // Run all power management initialisation callbacks
    call_functions_from_pointer(&POWER_INIT_CALLBACKS, &POWER_NUM_INIT_CALLBACKS);

    // Disable non-required hardware for power-saving
    // ADC, SPI, Timer{0,2,3,4,5}, TWI
    // Disable ADC completely
    ACSR |= _BV(ACD); // Disable A/D comparator
    ADCSRA &= ~_BV(ADEN); // Disable A/D converter
    DIDR0 = _BV(sizeof(DIDR0)) - 1; // Disable all A/D inputs
    DIDR1 = _BV(sizeof(DIDR1)) - 1; // Disable AIN0 and AIN1

    // Set according bits in the Power Reduction Registers
    PRR0 |= _BV(PRADC) | /* ADC */
            _BV(PRSPI) | /* SPI */
            _BV(PRTIM0) | _BV(PRTIM2) | /* Unused Timers */
            _BV(PRTWI); /* TWI */

    PRR1 |= _BV(PRTIM3) | _BV(PRTIM4) | _BV(PRTIM5); /* Unused timers */
}

void power_sleep() {
#ifdef DEBUG
    uart_sendmsg(DBG_UART, "[DBG] Switching to sleep mode\r\n", -1);
#endif
    // Run all power management sleep callbacks
    call_functions_from_pointer(&POWER_SLEEP_CALLBACKS, &POWER_NUM_SLEEP_CALLBACKS);

    // Disable watchdog timer before going to sleep
    wdt_disable();

    // Put CPU in powerdown mode
    SMCR |= (1<<SE)|(0<<SM2)|(1<<SM1)|(0<<SM0);
    asm("sleep");
}

void power_wakeup() {
    // Clear sleep enable bit
    SMCR &= ~_BV(SE);
    
    // Run all power management wakeup callbacks
    call_functions_from_pointer(&POWER_WAKEUP_CALLBACKS, &POWER_NUM_WAKEUP_CALLBACKS);

#ifdef DEBUG
    uart_sendmsg(DBG_UART, "[DBG] Waking up from sleep mode\r\n", -1);
#endif
}
