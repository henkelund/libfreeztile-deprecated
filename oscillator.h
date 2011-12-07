#ifndef _FZ_OSCILLATOR_H_
#define _FZ_OSCILLATOR_H_

#include "types.h"

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

#endif // _FZ_OSCILLATOR_H_