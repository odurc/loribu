
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "ringbuff.h"

int main(void)
{
#ifdef RINGBUFF_ONLY_STATIC_ALLOCATION
    uint8_t user_buffer[17];
    ringbuff_t *rb = ringbuff_create_from(user_buffer, 17);
#else
    ringbuff_t *rb = ringbuff_create(17);
#endif

    const char data[] = "hello dear world";
    uint32_t len = strlen(data);
    ringbuff_write(rb, (uint8_t *) data, len);

    // full, empty
    assert(ringbuff_is_full(rb));
    assert(!ringbuff_is_empty(rb));

    // used, available
    assert(ringbuffer_used_space(rb) == len);
    assert(ringbuff_available_space(rb) == 0);

    // count
    assert(ringbuff_count(rb, 'l') == 3);
    assert(ringbuff_count(rb, 'x') == 0);

    // peek
    char buffer[16];
    memset(buffer, 0, sizeof buffer);
    ringbuff_peek(rb, (uint8_t *) buffer, 5);
    assert(strcmp(buffer, "hello") == 0);
    assert(ringbuffer_used_space(rb) == len);

    // search
    assert(ringbuff_search(rb, (uint8_t *) "dear", 4) == 6);
    assert(ringbuff_search(rb, (uint8_t *) "sad", 3) == -1);

    // full, empty
    ringbuff_flush(rb);
    assert(!ringbuff_is_full(rb));
    assert(ringbuff_is_empty(rb));

    ringbuff_destroy(rb);

    return 0;
}
