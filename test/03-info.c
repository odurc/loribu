
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "loribu.h"

int main(void)
{
#ifdef LORIBU_ONLY_STATIC_ALLOCATION
    uint8_t user_buffer[17];
    loribu_t *rb = loribu_create_from(user_buffer, 17);
#else
    loribu_t *rb = loribu_create(17);
#endif

    const char data[] = "hello dear world";
    uint32_t len = strlen(data);
    loribu_write(rb, (uint8_t *) data, len);

    // full, empty
    assert(loribu_is_full(rb));
    assert(!loribu_is_empty(rb));

    // used, available
    assert(loribu_used_space(rb) == len);
    assert(loribu_available_space(rb) == 0);

    // count
    assert(loribu_count(rb, 'l') == 3);
    assert(loribu_count(rb, 'x') == 0);

    // peek
    char buffer[16];
    memset(buffer, 0, sizeof buffer);
    loribu_peek(rb, (uint8_t *) buffer, 5);
    assert(strcmp(buffer, "hello") == 0);
    assert(loribu_used_space(rb) == len);

    // search
    assert(loribu_search(rb, (uint8_t *) "dear", 4) == 6);
    assert(loribu_search(rb, (uint8_t *) "sad", 3) == -1);

    // full, empty
    loribu_flush(rb);
    assert(!loribu_is_full(rb));
    assert(loribu_is_empty(rb));

    loribu_destroy(rb);

    return 0;
}
