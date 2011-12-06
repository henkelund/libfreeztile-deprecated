#ifndef _FZ_FORM_H_
#define _FZ_FORM_H_

#include "types.h"

static const fz_result_t FZ_RESULT_SUCCESS         = 0;
static const fz_result_t FZ_RESULT_NOT_IMPLEMENTED = 1;
static const fz_result_t FZ_RESULT_MALLOC_ERROR    = 2;
static const fz_result_t FZ_RESULT_MUTEX_ERROR     = 3;

static const unsigned int FZ_FORM_STATE_NONE              = 0;
static const unsigned int FZ_FORM_STATE_APPLYING          = (1 << 0);

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
 * Allocate & initialize the given form struct
 * 
 * @param  fz_form_t **form
 * @return fz_result
 */
fz_result_t fz_form_create(fz_form_t **form);

/**
 * Free resources of the given form struct
 * 
 * @param  fz_form_t **form
 * @return fz_result
 */
fz_result_t fz_form_destroy(fz_form_t **form);

/**
 *
 * @param  fz_splbuf_t *samples
 * @param  fz_form_t *form
 * @return fz_result
 */
fz_result_t fz_form_apply(
                          fz_splbuf_t *samples,
                          fz_form_t   *form);

/**
 *
 * @param  fz_oscillator_t**
 * @return fz_result_t
 */
fz_result_t fz_oscillator_create(fz_oscillator_t **oscillator);

/**
 *
 * @param  fz_oscillator_t**
 * @return fz_result_t
 */
fz_result_t fz_oscillator_destroy(fz_oscillator_t **oscillator);

/**
 *
 * @param  fz_splbuf_t *samples
 * @param  fz_oscillator_t    *oscillator
 * @return fz_result_r
 */
fz_result_t fz_oscillator_apply(
                                fz_splbuf_t     *samples,
                                fz_oscillator_t *oscillator);

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
 * @param  fz_bezier_t *form
 * @return fz_result_t 
 */
fz_result_t fz_bezier_build_prototype(
                                      fz_bezier_t *bezier,
                                      fz_form_t   *form,
                                      fz_uint_t   size);

const char* fz_result_string(fz_result_t result);

#endif // _FZ_FORM_H_
