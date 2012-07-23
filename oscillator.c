/**
 * libfreeztile
 *
 * Copyright (C) 2012 Henrik Hedelund (henke.hedelund@gmail.com)
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
#include "util/list.h"

// ### PRIVATE ###

fz_ptr_t
_fz_osc_construct(
                  const fz_ptr_t  self,
                  va_list        *args)
{
    (void) args;
    fz_osc_t *_self     = (fz_osc_t*) self;
    _self->form         = fz_new(fz_form);
    _self->amp          = 1;
    _self->phase        = 0;
    _self->sample_rate  = 44100;
    return _self;
}

fz_ptr_t
_fz_osc_destruct(fz_ptr_t self)
{
    fz_osc_t *_self = (fz_osc_t*) self;
    fz_free(_self->form);
    return _self;
}

static const fz_object_t _fz_osc = {
    sizeof (fz_osc_t),
    _fz_osc_construct,
    _fz_osc_destruct,
    NULL,
    NULL
};

// ### PUBLIC ###

const fz_ptr_t fz_osc = (const fz_ptr_t) &_fz_osc;

fz_result_t
fz_oscillator_apply(
                    fz_octx_t *ctx,
                    fz_list_t *samples)
{
    fz_result_t result = FZ_RESULT_SUCCESS;
    fz_uint_t   i;
    fz_float_t  step_size;
    fz_real_t   frame;

    if (!fz_list_type(samples, fz_amp_t) || ctx->osc == NULL) {
        return FZ_RESULT_INVALID_ARG;
    }

    // sample rate divided by frequeny gives number of samples per period
    step_size = 1.f/(ctx->osc->sample_rate/ctx->freq);

    // fill the frame part of the sample buffer
    for (i = 0; i < samples->size; ++i) {
        frame = ctx->frame + ctx->osc->phase;
        while (frame >= 1) {
            frame -= 1;
        }
        fz_list_val(ctx->framebuf, i, fz_frame_t) = frame;
        ctx->frame += step_size;
        while (ctx->frame >= 1) {
            ctx->frame -= 1;
        }
    }

    result = fz_form_apply(
            ctx->osc->form, ctx->framebuf, samples);
    if (result != FZ_RESULT_SUCCESS) {
        return result;
    }

    for (i = 0; i < samples->size; ++i) {
        fz_list_val(samples, i, fz_amp_t) *= ctx->amp * ctx->osc->amp;
    }

    return result;
}
