#ifndef _FZ_FREEZTILE_H_
#define _FZ_FREEZTILE_H_

#include "types.h"

static const fz_result_t FZ_RESULT_SUCCESS         = 0;
static const fz_result_t FZ_RESULT_NOT_IMPLEMENTED = -(1 << 0);
static const fz_result_t FZ_RESULT_MALLOC_ERROR    = -(1 << 1);
static const fz_result_t FZ_RESULT_MUTEX_ERROR     = -(1 << 2);

/**
 * Allocate & initialize a mutex lock
 * 
 * @param  fz_lock_t **lock
 * @return fz_result
 */
fz_result_t fz_lock_create(fz_lock_t **lock);

/**
 * Destroy a mutex lock
 * 
 * @param  fz_lock_t **lock
 * @return fz_result
 */
fz_result_t fz_lock_destroy(fz_lock_t **lock);

/**
 * Acquire a mutex lock
 * 
 * @param  fz_lock_t *lock
 * @return fz_result
 */
fz_result_t fz_lock_acquire(fz_lock_t *lock);

/**
 * Release a mutex lock
 * 
 * @param  fz_lock_t *lock
 * @return fz_result
 */
fz_result_t fz_lock_release(fz_lock_t *lock);

/**
 *
 * @param  fz_splbuf_t**
 * @param  fz_uint_t 
 * @return fz_result_t
 */
fz_result_t fz_splbuf_create(
                             fz_splbuf_t **buffer,
                             fz_uint_t   size);
/**
 *
 * @param  fz_splbuf_t**
 * @return fz_result_t
 */
fz_result_t fz_splbuf_destroy(fz_splbuf_t **buffer);

/**
 *
 * @param  fz_splbuf_t*
 * @param  fz_uint_t
 * @return fz_result_t
 */
fz_result_t fz_splbuf_resize(
                             fz_splbuf_t *buffer,
                             fz_uint_t   size);

/**
 * 
 * @param  fz_result_t result
 * @return const char*
 */
const char* fz_result_string(fz_result_t result);

#endif // _FZ_FREEZTILE_H_