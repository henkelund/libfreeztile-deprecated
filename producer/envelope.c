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

#include "envelope.h"
#include "../freeztile.h"
#include "../form.h"
#include "../util/list.h"

static
fz_ptr_t
_fz_envdesc_construct(fz_ptr_t  self,
                      va_list  *args)
{
    (void) args;
    fz_envdesc_t *_self = (fz_envdesc_t*) self;
    fz_uint_t i;

    for (i = 0; i < 4; ++i) {
        _self->durations[i] = 0;
        _self->forms[i]     = fz_new(fz_form);
        fz_list_clear(_self->forms[i]->template, 1024); //FIXME: Default resolution
    }

    return _self;
}

static
fz_ptr_t
_fz_envdesc_destruct(fz_ptr_t self)
{
    fz_envdesc_t *_self = (fz_envdesc_t*) self;

    fz_uint_t i = 0;
    for (; i < 4; ++i) {
        fz_release(_self->forms[i]);
    }

    return _self;
}

static
fz_result_t
_fz_envelope_produce(
                     fz_producer_t *self,
                     fz_list_t     *buffer)
{
    fz_envelope_t *_self = (fz_envelope_t*) self;
    fz_form_t     *curve;
    fz_uint_t      i;
    fz_uint_t      frame;
    fz_amp_t       amp;

    if (_self->state == FZ_ENV_SILENT || _self->descriptor == NULL) {
        return fz_list_size(buffer);
    }

    curve = _self->descriptor->forms[_self->state];

    for (i = 0; i < fz_list_size(buffer); ++i, ++_self->frame) {
        if (_self->frame >= _self->descriptor->durations[_self->state]) {
            switch (_self->state) {
                case FZ_ENV_SUSTAIN:
                    amp = fz_list_size(curve->template) > 0 ?
                            fz_list_val(curve->template,
                                        fz_list_size(curve->template) - 1, fz_amp_t)
                            :
                            (i > 0 ? fz_list_val(buffer, i - 1, fz_amp_t) : 0.0);
                    for (; i < fz_list_size(buffer); ++i) {
                        fz_list_val(buffer, i, fz_amp_t) = amp;
                    }
                    break;
                case FZ_ENV_RELEASE:
                    for (; i < fz_list_size(buffer); ++i) {
                        fz_list_val(buffer, i, fz_amp_t) = 0.0;
                    }
                    _self->state = FZ_ENV_SILENT;
                    break;
                default:
                    curve = _self->descriptor->forms[++_self->state];
                    _self->frame = 0;
                    break;
            }
        }

        if (i == fz_list_size(buffer)) {
            break;
        }

        frame = (fz_uint_t) ((((fz_real_t) _self->frame) /
                                _self->descriptor->durations[_self->state]) *
                                        fz_list_size(curve->template));

        if (frame == fz_list_size(curve->template)) {
            if (fz_list_size(curve->template) > 0) {
                --frame;
            } else {
                continue;
            }
        }

        fz_list_val(buffer, i, fz_amp_t) +=
                fz_list_val(curve->template, frame, fz_amp_t);

        if (_self->state == FZ_ENV_RELEASE) {
            if (_self->frame == 0 && fz_list_val(buffer, i, fz_amp_t) > 0) {
                _self->factor /= fz_list_val(buffer, i, fz_amp_t);
            }
            fz_list_val(buffer, i, fz_amp_t) *= _self->factor;
        }
    }

    if (fz_list_size(buffer) > 0 && _self->state != FZ_ENV_RELEASE) {
        _self->factor = fz_list_val(buffer, i - 1, fz_amp_t);
    }

    return fz_list_size(buffer);
}

static
fz_ptr_t
_fz_envelope_construct(fz_ptr_t  self,
                       va_list  *args)
{
    fz_envelope_t *_self = (fz_envelope_t*) self;
    _self->descriptor    = va_arg(*args, fz_envdesc_t*);
    _self->state         = FZ_ENV_SILENT;
    _self->frame         = 0;
    _self->factor        = 1.0;

    if (_self->descriptor) {
        fz_retain(_self->descriptor);
    }

    ((fz_producer_t*) self)->produce = _fz_envelope_produce;

    return _self;
}

static
fz_ptr_t
_fz_envelope_destruct(fz_ptr_t self)
{
    fz_envelope_t *_self = (fz_envelope_t*) self;
    if (_self->descriptor != NULL) {
        fz_release(_self->descriptor);
    }
    return _self;
}

static const fz_object_t _fz_envdesc = {
    sizeof (fz_envdesc_t),
    _fz_envdesc_construct,
    _fz_envdesc_destruct,
    NULL,
    NULL
};

static const fz_object_t _fz_envelope = {
    sizeof (fz_envelope_t),
    _fz_envelope_construct,
    _fz_envelope_destruct,
    NULL,
    NULL
};

const fz_ptr_t fz_envdesc  = (const fz_ptr_t) &_fz_envdesc;
const fz_ptr_t fz_envelope = (const fz_ptr_t) &_fz_envelope;

void
fz_envelope_attack(fz_envelope_t *envelope)
{
    envelope->frame = 0;
    envelope->state = FZ_ENV_ATTACK;
}

void
fz_envelope_release(fz_envelope_t *envelope)
{
    envelope->frame = 0;
    envelope->state = FZ_ENV_RELEASE;
}

fz_result_t
fz_envelope_set_adsr_curve(
                           fz_envdesc_t *descriptor,
                           fz_curve_t   *curve,
                           fz_int_t      state)
{
    fz_result_t err;

    if (state < 0 || state > 3) {
        return FZ_RESULT_INVALID_ARG;
    }

    err = fz_curve_render(curve, descriptor->forms[state]);
    if (err != FZ_RESULT_SUCCESS) {
        return err;
    }

    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_envelope_set_adsr(
                     fz_envdesc_t *descriptor,
                     fz_amp_t      al,
                     fz_uint_t     ad,
                     fz_amp_t      dl,
                     fz_uint_t     dd,
                     fz_amp_t      sl,
                     fz_uint_t     sd,
                     fz_uint_t     rd)
{
    // FZ_ENV_ATTACK
    fz_curve_t attack = {
        .start = 0,
        .a     = {.x = 0,  .y = 0},
        .b     = {.x = 1., .y = al},
        .end   = al
    };
    fz_envelope_set_adsr_curve(descriptor, &attack, FZ_ENV_ATTACK);
    descriptor->durations[FZ_ENV_ATTACK] = ad;

    // FZ_ENV_DECAY
    fz_curve_t decay = {
        .start = al,
        .a     = {.x = 0,  .y = al},
        .b     = {.x = 1., .y = dl},
        .end   = dl
    };
    fz_envelope_set_adsr_curve(descriptor, &decay, FZ_ENV_DECAY);
    descriptor->durations[FZ_ENV_DECAY] = dd;

    // FZ_ENV_SUSTAIN
    fz_curve_t sustain = {
        .start = dl,
        .a     = {.x = 0,  .y = dl},
        .b     = {.x = 1., .y = sl},
        .end   = sl
    };
    fz_envelope_set_adsr_curve(descriptor, &sustain, FZ_ENV_SUSTAIN);
    descriptor->durations[FZ_ENV_SUSTAIN] = sd;

    // FZ_ENV_RELEASE
    fz_curve_t release = {
        .start = sl,
        .a     = {.x = 0,  .y = sl},
        .b     = {.x = 1., .y = 0},
        .end   = 0
    };
    fz_envelope_set_adsr_curve(descriptor, &release, FZ_ENV_RELEASE);
    descriptor->durations[FZ_ENV_RELEASE] = rd;

    return FZ_RESULT_SUCCESS;
}
