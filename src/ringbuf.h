#include <stdint.h>  // uint32_t
#include <stdbool.h>  // bool

#define RINGBUF_MAX_SIZE 512  // Maximum buffer size is 512 bytes

typedef struct {
    uint32_t rpos;  // Last read position
    uint32_t wpos;  // Last written position
    char ringbuf[RINGBUF_MAX_SIZE];  // Buffer containing the data
} ringbuffer;

void ringbuf_add_char(ringbuffer*, char);
bool ringbuf_has_new_data(ringbuffer*);
char ringbuf_read_char(ringbuffer*);
char ringbuf_last_char(ringbuffer*);
void ringbuf_reset(ringbuffer*);
void ringbuf_init(ringbuffer*);
