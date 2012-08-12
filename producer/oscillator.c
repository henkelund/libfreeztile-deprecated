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
#include "../freeztile.h"
#include "../form.h"
#include "../util/list.h"

// ### PRIVATE ###

static
fz_ptr_t
_fz_oscdesc_construct(
                  const fz_ptr_t  self,
                  va_list        *args)
{
    (void) args;
    fz_oscdesc_t *_self = (fz_oscdesc_t*) self;
    _self->form         = fz_new(fz_form);
    _self->amp          = 1;
    _self->phase        = 0;
    return _self;
}

static
fz_ptr_t
_fz_oscdesc_destruct(fz_ptr_t self)
{
    fz_oscdesc_t *_self = (fz_oscdesc_t*) self;
    fz_release(_self->form);
    return _self;
}

static
fz_result_t
_fz_oscillator_produce(
                       fz_producer_t *self,
                       fz_list_t     *buffer)
{
    fz_oscillator_t *_self  = (fz_oscillator_t*) self;
    fz_result_t      result = FZ_RESULT_SUCCESS;
    fz_uint_t        i;
    fz_float_t       step_size;
    fz_real_t        frame;

    if (_self->descriptor == NULL || _self->sample_rate == 0 || _self->freq == 0.) {
        return buffer->size;
    }

    // sample rate divided by frequeny gives number of samples per period
    step_size = 1.f/(_self->sample_rate/_self->freq);

    // fill the frame progress buffer
    fz_list_clear(_self->framebuf, buffer->size);
    for (i = 0; i < buffer->size; ++i) {
        frame = _self->frame + _self->descriptor->phase;
        while (frame >= 1.) {
            frame -= 1.;
        }
        fz_list_val(_self->framebuf, i, fz_frame_t) = frame;
        _self->frame += step_size;
        while (_self->frame >= 1.) {
            _self->frame -= 1.;
        }
    }

    result = fz_form_apply(_self->descriptor->form, _self->framebuf, buffer);
    if (result != FZ_RESULT_SUCCESS) {
        return -result;
    }

    for (i = 0; i < buffer->size; ++i) {
        fz_list_val(buffer, i, fz_amp_t) *= _self->descriptor->amp;
    }

    return buffer->size;
}

static
fz_ptr_t
_fz_oscillator_construct(
                         const fz_ptr_t  self,
                         va_list        *args)
{
    (void) args;
    fz_oscillator_t *_self = (fz_oscillator_t*) self;
    _self->descriptor      = NULL;
    _self->frame           = 0;
    _self->framebuf        = fz_list_new(fz_frame_t);
    _self->freq            = 0;
    _self->sample_rate     = 0;

    ((fz_producer_t*) self)->produce = _fz_oscillator_produce;

    return _self;
}

static
fz_ptr_t
_fz_oscillator_destruct(fz_ptr_t self)
{
    fz_oscillator_t *_self = (fz_oscillator_t*) self;
    fz_release(_self->framebuf);
    return _self;
}

static const fz_object_t _fz_oscdesc = {
    sizeof (fz_oscdesc_t),
    _fz_oscdesc_construct,
    _fz_oscdesc_destruct,
    NULL,
    NULL
};

static const fz_object_t _fz_oscillator = {
    sizeof (fz_oscillator_t),
    _fz_oscillator_construct,
    _fz_oscillator_destruct,
    NULL,
    NULL
};

// ### PUBLIC ###

const fz_ptr_t fz_oscdesc    = (const fz_ptr_t) &_fz_oscdesc;
const fz_ptr_t fz_oscillator = (const fz_ptr_t) &_fz_oscillator;
