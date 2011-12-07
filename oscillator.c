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
