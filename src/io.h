#ifndef IO_H
#define IO_H

#include "config.h"

#define IO_PORT_HIGH 1
#define IO_PORT_LOW 0

bool io_set_port_state(int port, int port_state);
bool io_get_port_state(int port);

#endif /* IO_H */
