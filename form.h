#ifndef _FZ_FORM_H_
#define _FZ_FORM_H_

#include <pthread.h>
#include "types.h"

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
 * @param  fz_bezier_form_t *form
 * @return fz_result_t 
 */
fz_result_t     fz_bezier_build_prototype(fz_bezier_form_t *bf);

#endif // _FZ_FORM_H_