#include <stdlib.h>
#include "oscillator.h"
#include "freeztile.h"

fz_result_t
fz_oscillator_create(fz_oscillator_t **oscillator)
{
    fz_result_t result;
    fz_oscillator_t *osc = malloc(sizeof(fz_oscillator_t));
    if (osc == NULL) {
        return FZ_RESULT_MALLOC_ERROR;
    }
    if ((result = fz_form_create(&osc->form)) != 0) {
        return result;
    }
    osc->instant = 0.0f;
    osc->amplitude = 1.0f;
    osc->frequency = 440.0f;
    osc->sample_rate = 44100;
    *oscillator = osc;
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_oscillator_destroy(fz_oscillator_t **oscillator)
{
    return FZ_RESULT_NOT_IMPLEMENTED;
}

fz_result_t     
fz_oscillator_apply(
                    fz_splbuf_t     *samples,
                    fz_oscillator_t *oscillator)
{
    fz_uint_t i;
    // calculate the progress rate
    fz_float_t step_size =  
        1.f/(oscillator->sample_rate/oscillator->frequency);
        // sample rate divided by frequeny gives number of
        // samples per period
    
    // fill the instants part of the sample buffer
    for (i = 0; i < samples->size; ++i) {
        samples->instants[i] = oscillator->instant;
        oscillator->instant += step_size;
        if (oscillator->instant >= 1.0f) {
            oscillator->instant -= 1.0f;
        }
    }

    fz_form_apply(samples, oscillator->form);
    return FZ_RESULT_SUCCESS;
}
