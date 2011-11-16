#include "form.h"

fz_result_t fz_init_form(fz_form_t *form)
{
    form->state = 
        FZ_FORM_STATE_CACHE_ACTIVATED | 
        FZ_FORM_STATE_CACHE_INVALIDATED;
    pthread_mutex_init(&(form->lock), NULL);
    return FZ_RESULT_SUCCESS;
}

void _fz_form_edit_start(fz_form_t *form)
{
    pthread_mutex_lock(&(form->lock));
}

void _fz_form_edit_end(fz_form_t *form)
{
    form->state |= FZ_FORM_STATE_CACHE_INVALIDATED;
    pthread_mutex_unlock(&(form->lock));
}
