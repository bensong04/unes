/**
 * @file graphics/mtx_buffer.h
 * @brief Interface for a mutexed buffer of pixels.
 * 
 * @author Benedict Song <benedict04song@gmail.com>
 */
#ifndef _MTX_BUFFER_INCLUDED
#include <pthread.h>
#include "graphics/gfxprimitives.h"
#include "graphics/pixels.h"

#define LOCK(mut) (pthread_mutex_lock(&mut))
#define UNLOCK(mut) (pthread_mutex_unlock(&mut))

typedef pthread_mutex_t mut_t;

/**
 * @brief Mutexed buffer of pixels for sharing between the two threads.
 */
typedef struct mtx_buffer {
    mut_t mtx;
    pixel_t buf[NES_PX_WIDTH * NES_PX_WIDTH];
} mtx_buffer_t;

/**
 * @brief Unmutexed bare buffer of pixels for either thread to own.
 */
typedef struct px_buffer {
    pixel_t buf[NES_PX_WIDTH * NES_PX_WIDTH];
} px_buffer_t;

/**
 * @brief
 * 
 * @note Acquires and releases a lock on the mutex.
 */
pixel_t get_at_coords(size_t x, size_t y);

/**
 * @brief Copies everything from the source buffer into the
 * destination buffer.
 * 
 * @note Acquires and releases a lock on the mutex.
*/
pixel_t flip(mtx_buffer_t *destbuf, px_buffer_t *srcbuf);

#define _MTX_BUFFER_INCLUDED
#endif