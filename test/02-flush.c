
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "loribu.h"

int main(void)
{
    uint8_t user_buffer[512];
    loribu_t *rb = loribu_create_from(user_buffer, 512);

    const char data[] = "hello world";
    int n = loribu_write(rb, (uint8_t *) data, sizeof data);
    assert(n == sizeof data);

    loribu_flush(rb);

    uint8_t buffer[128];
    int m = loribu_read(rb, buffer, n);
    assert(m == 0);

    loribu_destroy(rb);

    return 0;
}
