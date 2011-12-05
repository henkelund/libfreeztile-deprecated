#ifndef _FZ_FORM_H_
#define _FZ_FORM_H_

#include <pthread.h>
#include "types.h"

static const fz_result_t FZ_RESULT_SUCCESS         = 0;
static const fz_result_t FZ_RESULT_NOT_IMPLEMENTED = 1;

static const unsigned int FZ_FORM_STATE_NONE              = 0;
static const unsigned int FZ_FORM_STATE_APPLYING          = (1 << 0);

/**
 * Initialize the given form struct
 * 
 * @param  fz_form_t *form
 * @return fz_result
 */
fz_result_t     fz_form_init(fz_form_t *form);

/**
 *
 * @param  fz_sample_buffer_t *samples
 * @param  fz_form_t *form
 * @return fz_result
 */
fz_result_t     fz_form_apply(
                              fz_sample_buffer_t *samples,
                              fz_form_t          *form);

 /**
 *
 * @param  fz_oscillator_t*
 * @return fz_result_t
 */
fz_result_t     fz_oscillator_malloc(fz_oscillator_t *oscillator);

/**
 *
 * @param  fz_oscillator_t*
 * @return fz_result_t
 */
fz_result_t     fz_oscillator_free(fz_oscillator_t *oscillator);

/**
 *
 * @param  fz_sample_buffer_t *samples
 * @param  fz_oscillator_t    *oscillator
 * @return fz_result_r
 */
fz_result_t     fz_oscillator_apply(
                                    fz_sample_buffer_t *samples,
                                    fz_oscillator_t *oscillator);

/**
 * 
 * @param fz_form_t *form
 */
void            _fz_form_acquire(fz_form_t *form);

/**
 * 
 * @param fz_form_t *form
 */
void            _fz_form_release(fz_form_t *form);

/**
 * 
 * @param  fz_bezier_t *form
 * @return fz_result_t 
 */
fz_result_t     fz_bezier_build_prototype(
                                          fz_bezier_t *bezier,
                                          fz_form_t   *form,
                                          fz_uint_t   size);

const char*     fz_result_string(fz_result_t result);

#endif // _FZ_FORM_H_
