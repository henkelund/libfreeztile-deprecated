#ifndef _FZ_FORM_H_
#define _FZ_FORM_H_

#include "types.h"

static const unsigned int FZ_FORM_STATE_NONE       = 0;
static const unsigned int FZ_FORM_STATE_APPLYING   = (1 << 0);

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
 * @param  fz_bezier_t *form
 * @return fz_result_t 
 */
fz_result_t fz_bezier_build_prototype(
                                      fz_bezier_t *bezier,
                                      fz_form_t   *form,
                                      fz_uint_t   size);

#endif // _FZ_FORM_H_
