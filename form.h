#ifndef _FZ_FORM_H_
#define _FZ_FORM_H_

#include <pthread.h>
#include "types.h"

static const unsigned int FZ_FORM_STATE_NONE              = 0;
static const unsigned int FZ_FORM_STATE_APPLYING          = (1 << 0);
static const unsigned int FZ_FORM_STATE_CACHE_ACTIVATED   = (1 << 1);
static const unsigned int FZ_FORM_STATE_CACHE_INVALIDATED = (1 << 2);

/**
 * Initialize the given form struct
 * 
 * @param  fz_form_t *form
 * @return fz_result
 */
fz_result_t     fz_init_form(fz_form_t *form);

/**
 * 
 * @param fz_form_t *form
 */
void            _fz_form_edit_start(fz_form_t *form);

/**
 * 
 * @param fz_form_t *form
 */
void            _fz_form_edit_end(fz_form_t *form);

#endif // _FZ_FORM_H_