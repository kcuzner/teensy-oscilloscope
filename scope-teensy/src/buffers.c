/**
 * Shared oscilloscope memory buffers
 */
#include "buffers.h"

#define N_BUFFERS 16 //we have 32K of buffers...that's a lot of buffers

#define PREV(I) (I == 0 ? N_BUFFERS - 1 : I - 1)
#define NEXT(I) (I >= N_BUFFERS - 1 ? 0 : I + 1)

typedef enum { BUF_FREE, BUF_IN_USE, BUF_READY } buffer_state_t;

typedef struct {
    buffer_state_t state;
    uint16_t buffer[BUFFER_LENGTH];
} buffer_t;

// all the buffers!
static buffer_t buffers[N_BUFFERS];
static uint8_t last_free = 0;
static uint8_t last_ready = 0;

/**
 * Gets the next buffer with a given state
 * @param state State the buffer should be in
 * @param next State the buffer should be set to
 * @param last Last index to start at and update when looking through our buffers
 */
static uint16_t* buffer_get_next_like(buffer_state_t state, buffer_state_t next, uint8_t* last)
{
    uint8_t i;
    uint16_t* buf = NULL;

    DisableInterrupts;

    for (i = *last; i != PREV(*last); i = NEXT(i))
    {
        if (buffers[i].state == state)
        {
            *last = i;
            buffers[i].state = next;
            buf = buffers[i].buffer;
            goto done;
        }
    }

    done:
        EnableInterrupts;
        return buf;
}

void buffers_init(void)
{
}

uint16_t* buffers_get_next_free(uint8_t *index)
{
    uint16_t* buf = buffer_get_next_like(BUF_FREE, BUF_IN_USE, &last_free);
    *index = last_free;

    return buf;
}

void buffers_set_ready(uint8_t index)
{
    DisableInterrupts;
    if (buffers[index].state == BUF_IN_USE)
    {
        buffers[index].state = BUF_READY;
    }
    EnableInterrupts;
}

uint16_t* buffers_get_next_ready(uint8_t* index)
{
    uint16_t* buf = buffer_get_next_like(BUF_READY, BUF_READY, &last_ready);
    *index = last_ready;

    return buf;
}

void buffer_free(uint8_t index)
{
    DisableInterrupts;
    if (buffers[index].state == BUF_READY)
    {
        buffers[index].state = BUF_FREE;
    }
    EnableInterrupts;
}
