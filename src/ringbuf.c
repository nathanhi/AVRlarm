#include "ringbuf.h"

#include <string.h> // malloc
#include <stdlib.h> // memset
#include <stdio.h>
#include "uart.h"

void ringbuf_add_char(ringbuffer *buf, char c) {
    // Add a character to the ring buffer
    buf->ringbuf[(buf->wpos % RINGBUF_MAX_SIZE)] = c;
    buf->wpos++;
}

bool ringbuf_has_new_data(ringbuffer buf) {
    // Check if the buffer contains new data
    return (buf.rpos < buf.wpos);
}

char ringbuf_read_char(ringbuffer *buf) {
    // Read next available character from the buffer
    if (!ringbuf_has_new_data(*(buf))) {
        // Empty buffer; nothing new
        return '\0';
    }

    if ((buf->wpos - buf->rpos) > RINGBUF_MAX_SIZE) {
        // Begin on correct position on overrun
        buf->rpos = buf->wpos - RINGBUF_MAX_SIZE;
    }

    return buf->ringbuf[(buf->rpos++ % RINGBUF_MAX_SIZE)];
}

void ringbuf_init(ringbuffer *buf) {
    // Initialise given buffer object
    buf->rpos = 0;
    buf->wpos = 0;
    memset(buf->ringbuf, '\0', RINGBUF_MAX_SIZE);
}
