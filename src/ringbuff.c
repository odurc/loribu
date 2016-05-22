/*
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
************************************************************************************************************************
*       INCLUDE FILES
************************************************************************************************************************
*/

#include "ringbuff.h"


/*
************************************************************************************************************************
*       INTERNAL MACROS
************************************************************************************************************************
*/

// if alternative malloc/free is not defined then use functions from libc
#if !defined(RINGBUFF_NO_DYNAMIC_ALLOCATION) && !defined(MALLOC)
#include <stdlib.h>
#define MALLOC  malloc
#define FREE    free
#endif

// macros
#define RB_IS_FULL(rb)      (rb->tail == (rb->head + 1) % rb->size)
#define RB_IS_EMPTY(rb)     (rb->head == rb->tail)
#define RB_INC(rb,idx)      (rb->idx = (rb->idx + 1) % rb->size)


/*
************************************************************************************************************************
*       INTERNAL CONSTANTS
************************************************************************************************************************
*/


/*
************************************************************************************************************************
*       INTERNAL DATA TYPES
************************************************************************************************************************
*/

struct ringbuff_t {
    uint32_t head, tail;
    uint8_t *buffer;
    uint32_t size;
};


/*
************************************************************************************************************************
*       INTERNAL GLOBAL VARIABLES
************************************************************************************************************************
*/

#ifdef RINGBUFF_NUM_INSTANCES
ringbuff_t g_ringbuffers[RINGBUFF_NUM_INSTANCES];
#endif


/*
************************************************************************************************************************
*       INTERNAL FUNCTIONS
************************************************************************************************************************
*/


/*
************************************************************************************************************************
*       GLOBAL FUNCTIONS
************************************************************************************************************************
*/

#ifndef RINGBUFF_NO_DYNAMIC_ALLOCATION
ringbuff_t *ringbuff_create(uint32_t buffer_size)
{
    ringbuff_t *rb = (ringbuff_t *) MALLOC(sizeof(ringbuff_t));

    if (rb)
    {
        rb->head = 0;
        rb->tail = 0;
        rb->buffer = (uint8_t *) MALLOC(buffer_size);
        rb->size = buffer_size;

        // checks memory allocation
        if (!rb->buffer)
        {
            FREE(rb);
            rb = 0;
        }
    }

    return rb;
}
#endif

#ifdef RINGBUFF_NUM_INSTANCES
ringbuff_t *ringbuff_create_from(uint8_t *buffer, uint32_t buffer_size)
{
    if (buffer == 0)
        return 0;

    // search for free instance and return a ringbuffer
    for (uint32_t i = 0; i < RINGBUFF_NUM_INSTANCES; i++)
    {
        if (g_ringbuffers[i].buffer == 0)
        {
            ringbuff_t *rb = &g_ringbuffers[i];
            rb->head = 0;
            rb->tail = 0;
            rb->buffer = buffer;
            rb->size = buffer_size;

            return rb;
        }
    }

    return 0;
}
#endif

void ringbuff_destroy(ringbuff_t *rb)
{
#ifdef RINGBUFF_NUM_INSTANCES
    // for static buffer
    for (uint32_t i = 0; i < RINGBUFF_NUM_INSTANCES; i++)
    {
        if (rb == &g_ringbuffers[i])
        {
            rb->buffer = 0;
            return;
        }
    }
#endif

#ifndef RINGBUFF_NO_DYNAMIC_ALLOCATION
    // for dynamic allocated buffer
    if (rb->buffer)
        FREE(rb->buffer);

    FREE(rb);
#endif
}

uint32_t ringbuff_write(ringbuff_t *rb, const uint8_t *data, uint32_t data_size)
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

uint32_t ringbuff_read(ringbuff_t *rb, uint8_t *buffer, uint32_t size)
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

uint32_t ringbuff_read_until(ringbuff_t *rb, uint8_t *buffer, uint32_t buffer_size, uint8_t token)
{
    uint32_t bytes = 0;
    uint8_t *data = buffer, dummy;

    if (!buffer)
    {
        data = &dummy;
        buffer_size = rb->size;
    }

    // does not read buffer if there is no occurrences of the token
    if (ringbuff_count(rb, token) > 0)
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

void ringbuff_flush(ringbuff_t *rb)
{
    rb->head = 0;
    rb->tail = 0;
}

uint32_t ringbuffer_used_space(ringbuff_t *rb)
{
    return ((rb->head - rb->tail) % rb->size);
}

uint32_t ringbuff_available_space(ringbuff_t *rb)
{
    return (rb->size - ((rb->head - rb->tail) % rb->size) - 1);
}

uint32_t ringbuff_is_full(ringbuff_t *rb)
{
    return RB_IS_FULL(rb);
}

uint32_t ringbuff_is_empty(ringbuff_t *rb)
{
    return RB_IS_EMPTY(rb);
}

uint32_t ringbuff_count(ringbuff_t *rb, uint8_t byte)
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

void ringbuff_peek(ringbuff_t *rb, uint8_t *buffer, uint8_t peek_size)
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

int32_t ringbuff_search(ringbuff_t *rb, const uint8_t *to_search, uint32_t size)
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
