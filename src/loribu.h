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

#ifndef LORIBU_H
#define LORIBU_H


/*
****************************************************************************************************
*       INCLUDE FILES
****************************************************************************************************
*/

#include <stdint.h>


/*
****************************************************************************************************
*       MACROS
****************************************************************************************************
*/

// library version
#define LORIBU_VERSION      "1.0.0"

// macro to ask for the buffer creation
#define LORIBU_CREATE_BUFFER    0


/*
****************************************************************************************************
*       CONFIGURATION
****************************************************************************************************
*/

//#define LORIBU_ONLY_STATIC_ALLOCATION
//#define LORIBU_MAX_INSTANCES    4


/*
****************************************************************************************************
*       DATA TYPES
****************************************************************************************************
*/

/**
 * @struct loribu_t
 * An opaque structure representing a ring buffer object
 */
typedef struct loribu_t loribu_t;


/*
****************************************************************************************************
*       FUNCTION PROTOTYPES
****************************************************************************************************
*/

/**
 * @defgroup loribu_manipulation Manipulation Functions
 * Set of functions to manipulate the ring buffer.
 * @{
 */

/**
 * Create a ring buffer object
 *
 * The ring buffer object can be created from an existing buffer by
 * providing it through \a buffer parameter.
 * The macro LORIBU_CREATE_BUFFER should be used at \a buffer parameter
 * when the creation of a new dynamically allocated buffer is required.
 *
 * @param[in] buffer a static or previously allocated buffer or LORIBU_CREATE_BUFFER
 * @param[in] buffer_size size of the buffer to be created
 *
 * @return pointer to loribu object or NULL if memory allocation fail
 */
loribu_t *loribu_create(uint8_t *buffer, uint32_t buffer_size);

/**
 * Destroy a ring buffer object
 *
 * @param[in] rb loribu object pointer
 */
void loribu_destroy(loribu_t *rb);

/**
 * Write bytes in the ring buffer
 *
 * If \a data is NULL the ring buffer will be filled with zeros.
 *
 * @param[in] rb ring buffer to write to
 * @param[in] data data byte array
 * @param[in] data_size amount of bytes in the \a data array
 *
 * @return number of written bytes
 */
uint32_t loribu_write(loribu_t *rb, const uint8_t *data, uint32_t data_size);

/**
 * Read bytes from ring buffer
 *
 * A NULL \a buffer can be used to remove \a buffer_size bytes from ring buffer.
 *
 * @param[in] rb ring buffer to read from
 * @param[out] buffer buffer where data read will be stored
 * @param[in] size amount of bytes to read
 *
 * @return number of read bytes
 */
uint32_t loribu_read(loribu_t *rb, uint8_t *buffer, uint32_t size);

/**
 * Read bytes from ring buffer until find first occurrence of \a token
 *
 * Before read the ring buffer it'll check if token is available on it.
 * If not, no bytes will be read.
 * A NULL \a buffer can be used to remove all bytes before \a token
 * be found. In this case \a buffer_size is ignored.
 *
 * @param[in] rb ring buffer to read from
 * @param[out] buffer buffer where data read will be stored
 * @param[in] buffer_size amount of bytes that \a buffer is able to store
 * @param[in] token a single byte to compare with the read byte
 *
 * @return number of read bytes
 */
uint32_t loribu_read_until(loribu_t *rb, uint8_t *buffer, uint32_t buffer_size, uint8_t token);

/**
 * Flush out all data of the ring buffer
 *
 * Actually, the ring buffer will have its indexes zeroed
 * and new incoming data will overwrite old data.
 *
 * @param[in] rb ring buffer to flush
 */
void loribu_flush(loribu_t *rb);

/**
 * @}
 * @defgroup loribu_information Information Functions
 * Set of functions to query information of the ring buffer.
 * These functions do not affect the data buffer.
 * @{
 */

/**
 * Returns the amount of used space
 *
 * In other words, it returns how many non read bytes are in the ring buffer.
 *
 * @param[in] rb ring buffer to check
 *
 * @return amount of used bytes of the ring buffer
 */
uint32_t loribu_used_space(loribu_t *rb);

/**
 * Returns the amount of avaiable space
 *
 * In other words, it returns how many bytes are free in the ring buffer.
 *
 * @param[in] rb ring buffer to check
 *
 * @return amount of available bytes of the ring buffer
 */
uint32_t loribu_available_space(loribu_t *rb);

/**
 * Check if ring buffer is full
 *
 * @param[in] rb ring buffer to check
 *
 * @return non zero if buffer is full
 */
uint32_t loribu_is_full(loribu_t *rb);

/**
 * Check if ring buffer is empty
 *
 * @param[in] rb ring buffer to check
 *
 * @return non zero if buffer is empty
 */
uint32_t loribu_is_empty(loribu_t *rb);

/**
 * Count how many \a byte are in the ring buffer
 *
 * @param[in] rb ring buffer to check
 * @param[in] byte the byte to compare in the counting
 *
 * @return amount of ocurrencies of \a byte in the ring buffer
 */
uint32_t loribu_count(loribu_t *rb, uint8_t byte);

/**
 * Read bytes from ring buffer without remove them
 *
 * This is useful when you want to check if a sequence of bytes
 * is available in the ring buffer before take the data.
 *
 * Note that \a loribu_peek doesn't check the buffer size, this means
 * \a peek_size must fit in the size of passed buffer.
 *
 * @param[in] rb ring buffer to peek
 * @param[out] buffer buffer where data read will be stored
 * @param[in] peek_size number of bytes to peek
 */
void loribu_peek(loribu_t *rb, uint8_t *buffer, uint8_t peek_size);

/**
 * Search for a sequence of bytes
 *
 * If the sequence is not available in the ring buffer \a -1 will be returned.
 * Otherwise, the return will be the number of bytes available before the first byte
 * of the sequence begin.
 *
 * @param[in] rb ring buffer where to search in
 * @param[in] to_search bytes sequence to search for
 * @param[in] size size of the sequence
 *
 * @return position of the first byte of the sequence
 */
int32_t loribu_search(loribu_t *rb, const uint8_t *to_search, uint32_t size);

/**
 * @}
 */

/*
****************************************************************************************************
*       CONFIGURATION ERRORS
****************************************************************************************************
*/

#if defined(LORIBU_ONLY_STATIC_ALLOCATION) && !defined(LORIBU_MAX_INSTANCES)
#error "LORIBU_ONLY_STATIC_ALLOCATION requires LORIBU_MAX_INSTANCES macro definition."
#endif


#endif
