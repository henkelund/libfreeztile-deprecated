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

    // Init to basic sample ADSR shape
    _self->curves[FZ_ENV_ATTACK].start = 0.;
    _self->curves[FZ_ENV_ATTACK].a.x   = 0.;
    _self->curves[FZ_ENV_ATTACK].a.y   = 0.;
    _self->curves[FZ_ENV_ATTACK].b.x   = 1.;
    _self->curves[FZ_ENV_ATTACK].b.y   = 1.;
    _self->curves[FZ_ENV_ATTACK].end   = 1.;

    _self->curves[FZ_ENV_DECAY].start = 1.;
    _self->curves[FZ_ENV_DECAY].a.x   = 0.;
    _self->curves[FZ_ENV_DECAY].a.y   = 1.;
    _self->curves[FZ_ENV_DECAY].b.x   = 1.;
    _self->curves[FZ_ENV_DECAY].b.y   = .5;
    _self->curves[FZ_ENV_DECAY].end   = .5;

    _self->curves[FZ_ENV_SUSTAIN].start = .5;
    _self->curves[FZ_ENV_SUSTAIN].a.x   = 0.;
    _self->curves[FZ_ENV_SUSTAIN].a.y   = .5;
    _self->curves[FZ_ENV_SUSTAIN].b.x   = 1.;
    _self->curves[FZ_ENV_SUSTAIN].b.y   = .75;
    _self->curves[FZ_ENV_SUSTAIN].end   = .75;

    _self->curves[FZ_ENV_RELEASE].start = .75;
    _self->curves[FZ_ENV_RELEASE].a.x   = 0.;
    _self->curves[FZ_ENV_RELEASE].a.y   = .75;
    _self->curves[FZ_ENV_RELEASE].b.x   = 1.;
    _self->curves[FZ_ENV_RELEASE].b.y   = 0.;
    _self->curves[FZ_ENV_RELEASE].end   = 0.;

    fz_uint_t i = 0;
    for (; i < 4; ++i) {
        _self->durations[i] = 256; //FIXME: ADSR default durations
        _self->cache[i]     = fz_new(fz_form);
        fz_list_clear(_self->cache[i]->template, 1024); //FIXME: Default resolution
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
        fz_free(_self->cache[i]);
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

    if (_self->state == FZ_ENV_SILENT) {
        fz_list_clear(buffer, buffer->size);
        return buffer->size;
    }

    curve = _self->descriptor->cache[_self->state];
    fz_curve_render(&_self->descriptor->curves[_self->state], curve);

    for (i = 0; i < buffer->size; ++i, ++_self->frame) {
        if (_self->frame >= _self->descriptor->durations[_self->state]) {
            switch (_self->state) {
                case FZ_ENV_SUSTAIN:
                    amp = curve->template->size > 0 ?
                            fz_list_val(
                                curve->template, curve->template->size - 1, fz_amp_t)
                            :
                            (i > 0 ? fz_list_val(buffer, i - 1, fz_amp_t) : 0.0);
                    for (; i < buffer->size; ++i) {
                        fz_list_val(buffer, i, fz_amp_t) = amp;
                    }
                    break;
                case FZ_ENV_RELEASE:
                    for (; i < buffer->size; ++i) {
                        fz_list_val(buffer, i, fz_amp_t) = 0.0;
                    }
                    _self->state = FZ_ENV_SILENT;
                    break;
                default:
                    curve = _self->descriptor->cache[++_self->state];
                    fz_curve_render(&_self->descriptor->curves[_self->state], curve);
                    _self->frame = 0;
                    break;
            }
        }

        if (i == buffer->size) {
            break;
        }

        frame = (fz_uint_t) ((((fz_real_t) _self->frame) /
                                _self->descriptor->durations[_self->state]) *
                                    curve->template->size);

        if (frame == curve->template->size) {
            if (curve->template->size > 0) {
                --frame;
            } else {
                continue;
            }
        }

        fz_list_val(buffer, i, fz_amp_t) =
                fz_list_val(curve->template, frame, fz_amp_t);

        if (_self->state == FZ_ENV_RELEASE) {
            if (_self->frame == 0 && fz_list_val(buffer, i, fz_amp_t) > 0) {
                _self->factor /= fz_list_val(buffer, i, fz_amp_t);
            }
            fz_list_val(buffer, i, fz_amp_t) *= _self->factor;
        }
    }

    if (buffer->size > 0 && _self->state != FZ_ENV_RELEASE) {
        _self->factor = fz_list_val(buffer, i - 1, fz_amp_t);
    }

    return buffer->size;
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
    } else {
        _self->descriptor = fz_new(fz_envdesc);
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
        fz_free(_self->descriptor);
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
