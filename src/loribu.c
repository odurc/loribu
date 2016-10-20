/*
 * loribu - Lord of the Ring Buffers
 * https://github.com/ricardocrudo/loribu
 *
 * Copyright (c) 2016 Ricardo Crudo <ricardo.crudo@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
****************************************************************************************************
*       INCLUDE FILES
****************************************************************************************************
*/

#include "loribu.h"


/*
****************************************************************************************************
*       INTERNAL MACROS
****************************************************************************************************
*/

// uses standard libc malloc/free functions if custom ones aren't defined
#if !defined(LORIBU_ONLY_STATIC_ALLOCATION) && !defined(MALLOC)
#include <stdlib.h>
#define MALLOC      malloc
#define FREE        free
#endif

// ensures MALLOC macro returns null if none allocation function is available
#if defined(LORIBU_ONLY_STATIC_ALLOCATION) && !defined(MALLOC)
#define MALLOC(n)   (0)
#endif

// uses internal functions if configured to use static allocation
#ifdef LORIBU_ONLY_STATIC_ALLOCATION
#define RB_ALLOC    rb_take
#define RB_FREE     rb_give
#define BUF_FREE    (void)

// uses macro defined functions if configured to use dynamic allocation
#else
#define RB_ALLOC    MALLOC
#define RB_FREE     FREE
#define BUF_FREE    FREE
#endif

#define RB_IS_FULL(rb)      (rb->tail == (rb->head + 1) % rb->size)
#define RB_IS_EMPTY(rb)     (rb->head == rb->tail)
#define RB_INC(rb,idx)      (rb->idx = (rb->idx + 1) % rb->size)


/*
****************************************************************************************************
*       INTERNAL CONSTANTS
****************************************************************************************************
*/


/*
****************************************************************************************************
*       INTERNAL DATA TYPES
****************************************************************************************************
*/

struct loribu_t {
    uint32_t head, tail;
    uint8_t *buffer;
    uint32_t size;
    int user_buffer;
};


/*
****************************************************************************************************
*       INTERNAL GLOBAL VARIABLES
****************************************************************************************************
*/

#ifdef LORIBU_ONLY_STATIC_ALLOCATION
loribu_t g_ringbuffers[LORIBU_MAX_INSTANCES];
#endif


/*
****************************************************************************************************
*       INTERNAL FUNCTIONS
****************************************************************************************************
*/

#ifdef LORIBU_ONLY_STATIC_ALLOCATION
static inline void *rb_take(int n)
{
    // unused parameter
    // it's here to make the function prototype compatible with malloc
    (void) n;

    static unsigned int rb_counter;

    // first time ring buffers are requested
    if (rb_counter < LORIBU_MAX_INSTANCES)
    {
        loribu_t *rb  = &g_ringbuffers[rb_counter++];
        return rb;
    }

    // iterate all array searching for a free spot
    // a ring buffer is considered free when its buffer is null
    for (int i = 0; i < LORIBU_MAX_INSTANCES; i++)
    {
        loribu_t *rb  = &g_ringbuffers[i];
        if (!rb->buffer)
            return rb;
    }

    return 0;
}

static inline void rb_give(void *rb)
{
    if (rb)
    {
        loribu_t *self = rb;
        self->buffer = 0;
    }
}
#endif


/*
****************************************************************************************************
*       GLOBAL FUNCTIONS
****************************************************************************************************
*/

loribu_t *loribu_create(uint8_t *buffer, uint32_t buffer_size)
{
    loribu_t *rb = (loribu_t *) RB_ALLOC(sizeof(loribu_t));

    if (rb)
    {
        rb->head = 0;
        rb->tail = 0;
        rb->user_buffer = 0;
        rb->size = buffer_size;

        if (buffer)
        {
            rb->user_buffer = 1;
            rb->buffer = buffer;
        }
        else
        {
            rb->buffer = MALLOC(buffer_size);
            if (!rb->buffer)
            {
                RB_FREE(rb);
                rb = 0;
            }
        }
    }

    return rb;
}

void loribu_destroy(loribu_t *rb)
{
    if (rb)
    {
        if (rb->buffer && !rb->user_buffer)
            BUF_FREE(rb->buffer);

        RB_FREE(rb);
    }
}

uint32_t loribu_write(loribu_t *rb, const uint8_t *data, uint32_t data_size)
{
    uint32_t bytes = 0;
    const uint8_t *pdata, dummy = 0;

    pdata = data ? data : &dummy;

    // write until all data be written or ring buffer be full
    while (data_size > 0 && !RB_IS_FULL(rb))
    {
        rb->buffer[rb->head] = *pdata;
        RB_INC(rb, head);

        if (data)
            pdata++;

        data_size--;
        bytes++;
    }

    return bytes;
}

uint32_t loribu_read(loribu_t *rb, uint8_t *buffer, uint32_t size)
{
    uint32_t bytes = 0;
    uint8_t *data, dummy;

    data = buffer ? buffer : &dummy;

    // read until requested size be reached or ring buffer be empty
    while (size > 0 && !RB_IS_EMPTY(rb))
    {
        *data = rb->buffer[rb->tail];
        RB_INC(rb, tail);

        if (buffer)
            data++;

        size--;
        bytes++;
    }

    return bytes;
}

uint32_t loribu_read_until(loribu_t *rb, uint8_t *buffer, uint32_t buffer_size, uint8_t token)
{
    uint32_t bytes = 0;
    uint8_t *data = buffer, dummy;

    if (!buffer)
    {
        data = &dummy;
        buffer_size = rb->size;
    }

    // does not read buffer if there is no occurrences of the token
    if (loribu_count(rb, token) > 0)
    {
        // read until find the token or ring buffer be empty
        while (buffer_size > 0 && !RB_IS_EMPTY(rb))
        {
            *data = rb->buffer[rb->tail];
            RB_INC(rb, tail);

            buffer_size--;
            bytes++;

            if (*data == token)
                break;

            if (buffer)
                data++;
        }
    }

    return bytes;
}

void loribu_flush(loribu_t *rb)
{
    rb->head = 0;
    rb->tail = 0;
}

uint32_t loribu_used_space(loribu_t *rb)
{
    return ((rb->head - rb->tail) % rb->size);
}

uint32_t loribu_available_space(loribu_t *rb)
{
    return (rb->size - ((rb->head - rb->tail) % rb->size) - 1);
}

uint32_t loribu_is_full(loribu_t *rb)
{
    return RB_IS_FULL(rb);
}

uint32_t loribu_is_empty(loribu_t *rb)
{
    return RB_IS_EMPTY(rb);
}

uint32_t loribu_count(loribu_t *rb, uint8_t byte)
{
    uint32_t count = 0;
    uint32_t tail, head;
    uint8_t data;

    tail = rb->tail;
    head = rb->head;

    while (tail != head)
    {
        data = rb->buffer[tail];
        tail = (tail + 1) % rb->size;

        if (data == byte)
            count++;
    }

    return count;
}

void loribu_peek(loribu_t *rb, uint8_t *buffer, uint8_t peek_size)
{
    uint32_t tail, head;
    uint8_t *data = buffer;

    tail = rb->tail;
    head = rb->head;

    while (peek_size > 0 && tail != head)
    {
        *data++ = rb->buffer[tail];
        tail = (tail + 1) % rb->size;
        peek_size--;
    }
}

int32_t loribu_search(loribu_t *rb, const uint8_t *to_search, uint32_t size)
{
    uint32_t tail, head;
    uint32_t count = 0;

    tail = rb->tail;
    head = rb->head;

    if (!to_search)
        return -1;

    while (tail != head)
    {
        const uint8_t *s = to_search;
        uint8_t data;

        // search for first byte
        do {
            data = rb->buffer[tail];
            tail = (tail + 1) % rb->size;
            count++;
        } while (data != *s && tail != head);

        if (data == *s && size == 1)
            return (count - 1);

        // check next bytes
        uint32_t i = size - 1, a = 1;
        while (tail != head)
        {
            data = rb->buffer[tail];
            tail = (tail + 1) % rb->size;

            s++;
            if (data != *s)
            {
                count += a;
                break;
            }

            a++;

            if (--i == 0)
                return (count - 1);
        }
    }

    return -1;
}
