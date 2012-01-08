/**
 * libfreeztile
 *
 * Copyright (C) 2011 Henrik Hedelund (henke.hedelund@gmail.com)
 *
 * This file is part of libfreeztile.
 *
 * libfreeztile is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libfreeztile is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libfreeztile. If not, see <http://www.gnu.org/licenses/>.
 *
 * @category libfreeztile
 * @package libfreeztile
 * @author Henrik Hedelund <henke.hedelund@gmail.com>
 * @copyright 2012 Henrik Hedelund (henke.hedelund@gmail.com)
 * @license http://www.gnu.org/licenses/gpl.html GNU GPL
 * @link freeztile.org
 */

#include <stdlib.h>
#include "oscillator.h"
#include "freeztile.h"
#include "form.h"
#include "list.h"

fz_result_t
fz_oscillator_create(fz_osc_t **oscillator)
{
    fz_result_t result;
    fz_osc_t *osc = malloc(sizeof(fz_osc_t));
    if (osc == NULL) {
        return FZ_RESULT_MALLOC_ERROR;
    }
    if ((result = fz_form_create(&osc->form)) != FZ_RESULT_SUCCESS) {
        free(osc);
        return result;
    }
    if ((result = fz_list_new(osc->frame_buffer, fz_frame_t))
            != FZ_RESULT_SUCCESS) {
        free(osc);
        fz_form_destroy(&osc->form);
        return result;
    }
    osc->amplitude   = 1;
    osc->phase       = 0;
    osc->sample_rate = 44100;
    *oscillator      = osc;
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_oscillator_destroy(fz_osc_t **oscillator)
{
    (void)oscillator;
    return FZ_RESULT_NOT_IMPLEMENTED;
}

fz_result_t     
fz_oscillator_apply(
                    fz_osc_t    *oscillator,
                    fz_ostate_t *state,
                    fz_list_t   *samples)
{
    fz_result_t result = FZ_RESULT_SUCCESS;
    fz_uint_t   i;
    fz_float_t  step_size;
    fz_real_t   frame;
    
    if (!fz_list_type(samples, fz_amp_t)) {
        return FZ_RESULT_INVALID_ARG;
    }

    result = fz_list_clear(oscillator->frame_buffer, samples->size);
    if (result != FZ_RESULT_SUCCESS) {
        return result;
    }
    
    // sample rate divided by frequeny gives number of samples per period
    step_size = 1.f/(oscillator->sample_rate/state->frequency);
    
    // fill the frame part of the sample buffer
    for (i = 0; i < samples->size; ++i) {
        frame = state->frame + oscillator->phase;
        while (frame >= 1) {
            frame -= 1;
        }
        fz_list_val(oscillator->frame_buffer, i, fz_frame_t) = frame;
        state->frame += step_size;
        while (state->frame >= 1) {
            state->frame -= 1;
        }
    }

    result = fz_form_apply(oscillator->form, oscillator->frame_buffer, samples);
    if (result != FZ_RESULT_SUCCESS) {
        return result;
    }
    
    for (i = 0; i < samples->size; ++i) {
        fz_list_val(samples, i, fz_amp_t) *= oscillator->amplitude;
    }
    
    return result;
}
