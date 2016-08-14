
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "ringbuff.h"

int main(void)
{
    ringbuff_t *rb = ringbuff_create(512);

    const char data[] = "hello world";
    int n = ringbuff_write(rb, (uint8_t *) data, sizeof data);
    assert(n == sizeof data);

    uint8_t buffer[128];
    int m = ringbuff_read(rb, buffer, n);
    assert(n == m);

    n = ringbuff_write(rb, (uint8_t *) data, strlen(data));
    n += ringbuff_write(rb, (uint8_t *) data, strlen(data) + 1);
    m = ringbuff_read_until(rb, buffer, sizeof buffer, 0);
    assert(n == m);

    ringbuff_destroy(rb);

    return 0;
}
