#ifndef IO_H
#define IO_H

#include "config.h"
#include <stdbool.h>

// Port states
#define IO_PORT_HIGH 1
#define IO_PORT_LOW 0

// Port directions
#define IO_PORT_IN 0
#define IO_PORT_OUT 1

bool io_port_init(int port, int port_direction, bool enable_pullup);
bool io_set_port_state(int port, int port_state);
bool io_get_port_state(int port);

#endif /* IO_H */
