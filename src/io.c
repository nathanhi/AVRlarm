#include "io.h"

int CURPORT = -1;

bool io_port_init(int port) {
    /* Initializes a given port and
     * points data registers to correct
     * port */
    if (port < PORTB0 && port > PORTB5) {
        // Invalid port specified
        return false;
    }
    
    if (CURPORT == port) {
        // Current port already initialized
        return true;
    }
    
    DDRB |= _BV(port);
    return true;
}

bool io_set_port_state(int port, int port_state) {
    if (CURPORT != port) {
        // Initialize port if necessary
        if (!io_port_init(port)) {
            return false;
        }
    }

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
