
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "ringbuff.h"

int main(void)
{
    uint8_t user_buffer[512];
    ringbuff_t *rb = ringbuff_create_from(user_buffer, 512);

    const char data[] = "hello world";
    int n = ringbuff_write(rb, (uint8_t *) data, sizeof data);
    assert(n == sizeof data);

    ringbuff_flush(rb);

    uint8_t buffer[128];
    int m = ringbuff_read(rb, buffer, n);
    assert(m == 0);

    ringbuff_destroy(rb);

    return 0;
}
