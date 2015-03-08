/**
 * Shared oscilloscope memory buffers
 *
 * These operate in a series of states, controlled by the oscilloscope and the
 * USB module. Initially, the oscilloscope will request a buffer. At this point,
 * the buffer is marked as "in-use". After the oscilloscope has filled the
 * buffer, it is set as "ready" (still "in-use"). When the USB module calls
 * buffers_get_next_ready, the next ready buffer is returned. Once the USB
 * module has transmitted the buffer, buffers_free can be called and the buffer
 * is returned to the free pool.
 */

#ifndef _BUFFERS_H_
#define _BUFFERS_H_

#include "arm_cm4.h"

#define BUFFER_LENGTH 1023 //we avoid 1024 because of a stupid 2048-length bug

/**
 * Initializes the shared buffers
 */
void buffers_init(void);

/**
 * Gets the next free buffer. This marks the buffer as in-use, but not ready.
 *
 * @param index Variable to be filled with the index of this buffer
 * @return The next buffer or NULL if none available
 */
uint16_t* buffers_get_next_free(uint8_t *index);

/**
 * Marks a buffer as ready
 * @param index Buffer index
 */
void buffers_set_ready(uint8_t index);

/**
 * Gets the next ready buffer. The buffer state is not changed.
 *
 * @param index Variable to be filled with the index of this buffer
 * @return The next buffer or NULL if none available
 */
uint16_t* buffers_get_next_ready(uint8_t* index);

/**
 * Frees a buffer
 * @param index Buffer index to mark as free
 */
void buffer_free(uint8_t index);

#endif // _BUFFERS_H_

