#include "io.h"

#define IO_PORT_IN 0
#define IO_PORT_OUT 1

int CURPORT = -1;
int CURPORTDIR = -1;

bool io_port_init(int port, int port_direction) {
    /* Initializes a given port and
     * points data registers to correct
     * port
     */
    if (port < PORTB0 && port > PORTB7) {
        // Invalid port specified
        return false;
    }

    if (CURPORT == port && CURPORTDIR == port_direction) {
        // Current port already initialized in right direction
        return true;
    }

    switch(port_direction) {
        // IO port is used for input
        case IO_PORT_IN: DDRB &= ~_BV(port); break;
        // IO port is used for output
        case IO_PORT_OUT: DDRB |= _BV(port); break;
        // Invalid port direction specified
        default: return false;
    }

    CURPORT = port;
    CURPORTDIR = port_direction;
    return true;
}

bool io_set_port_state(int port, int port_state) {
    /* Sets desired port state (IO_PORT_HIGH,
     * IO_PORT_LOW) on given port (PORTB{0-7\})
     */
    // Initialize port if necessary
    if (!io_port_init(port, IO_PORT_OUT))
        return false;

    switch(port_state) {
        // Set given port to LOW
        case IO_PORT_LOW: PORTB &= ~_BV(port); break;
        // Set given port to HIGH
        case IO_PORT_HIGH: PORTB |= _BV(port); break;
        // Invalid port state, return false
        default: return false;
    }

    return true;
}

bool io_get_port_state(int port) {
    /* Retrieves current state of
     * given port (IO_PORT_{LOW,HIGH})
     */
    // Initialize port if necessary
    if (!io_port_init(port, IO_PORT_IN))
        return false;

    // Shift to given port and get first bit
    return (PINB >> (port) & 0x1);
}
