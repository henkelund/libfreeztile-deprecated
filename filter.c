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
#include <math.h>
#include "filter.h"
#include "freeztile.h"
#include "list.h"
#include "form.h"

// ### PRIVATE ###

fz_ptr_t
_fz_filter_construct(
                    const fz_ptr_t  self,
                    va_list        *args)
{
    (void) args;
    fz_filter_t *_self = (fz_filter_t*) self;
    _self->filter      = NULL;
    _self->options     = FZ_FILTER_OPT_NONE;
    // env_buffer won't be here for long so 
    // no need to write filter destructor right now
    _self->env_buffer  = fz_list_new(fz_real_t);
    return self;
}

fz_uint_t
_fz_lowpass_filter(
                  fz_ptr_t  filter,
                  fz_list_t *samples)
{
    fz_filter_t  *flt = (fz_filter_t*)  filter;
    fz_lowpass_t *lp  = (fz_lowpass_t*) filter;
    fz_uint_t i;
    fz_float_t dt = samples->size, a;
    if (samples->size == 0) {
        return 0;
    }
    dt = (fz_float_t)samples->size;
    a = dt/(lp->rc*fz_list_val(flt->env_buffer, 0, fz_real_t) + dt);

    // calc first amplitude from stored history
    fz_list_val(samples, 0, fz_amp_t) =
        lp->last + (a*(fz_list_val(samples, 0, fz_amp_t) - lp->last));

    for (i = 1; i < samples->size; ++i) {
        a = dt/(lp->rc*fz_list_val(flt->env_buffer, i, fz_real_t) + dt);
        fz_list_val(samples, i, fz_amp_t) =
            fz_list_val(samples, i - 1, fz_amp_t) +
                (
                    a*
                        (
                            fz_list_val(samples, i, fz_amp_t) -
                            fz_list_val(samples, i - 1, fz_amp_t)
                         )
                );
        // i.e. samples[i] = samples[i-1] + α * (samples[i] - samples[i-1]);
    }
    lp->last = fz_list_val(samples, samples->size - 1, fz_amp_t);
    return samples->size;
}

fz_ptr_t
_fz_lowpass_construct(
                     const fz_ptr_t  self,
                     va_list        *args)
{
    fz_lowpass_t *_self = 
            ((const fz_object_t*) fz_filter)->construct(self, args);
    ((fz_filter_t*) _self)->filter = _fz_lowpass_filter;
    _self->rc   = 0;
    _self->last = 0;
    return _self;
}

// static filter object descriptor
static const fz_object_t _fz_filter = {
    sizeof (fz_filter_t),
    _fz_filter_construct,
    NULL,
    NULL,
    NULL
};

static const fz_object_t _fz_lowpass = {
    sizeof (fz_lowpass_t),
    _fz_lowpass_construct,
    NULL,
    NULL,
    NULL
};

// ### PUBLIC ###

const fz_ptr_t fz_filter =  (const fz_ptr_t) &_fz_filter;
const fz_ptr_t fz_lowpass = (const fz_ptr_t) &_fz_lowpass;

fz_uint_t
fz_filter_apply(
                fz_filter_t *filter, 
                fz_list_t   *samples)
{
    fz_uint_t i;
    if (filter->filter != NULL) {
        // @todo if opt use env -> progress env
        fz_list_clear(filter->env_buffer, samples->size);
        for (i = 0; i < filter->env_buffer->size; ++i) {
            fz_list_val(filter->env_buffer, i, fz_real_t) = 1;
        }
        return filter->filter(filter, samples);
    }
    return 0;
}