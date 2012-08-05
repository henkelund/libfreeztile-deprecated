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

#include "amplifier.h"
#include "../util/list.h"

static
fz_int_t
_fz_amplifier_filtrate(
                       fz_filter_t *self,
                       fz_list_t   *buffer)
{
    fz_amplifier_t *_self = (fz_amplifier_t*) self;
    fz_uint_t       i     = 0;

    if (self->regulator && self->regbuf->size >= buffer->size) {
        for (; i < buffer->size; ++i) {
            fz_list_val(buffer, i, fz_amp_t) *=
                    _self->level*fz_list_val(self->regbuf, i, fz_real_t);
        }
    } else {
        for (; i < buffer->size; ++i) {
            fz_list_val(buffer, i, fz_amp_t) *= _self->level;
        }
    }

    return buffer->size;
}

static
fz_ptr_t
_fz_amplifier_construct(
                        fz_ptr_t  self,
                        va_list  *args)
{
    fz_amplifier_t *_self =
            (fz_amplifier_t*) ((fz_object_t*) fz_filter)->construct(self, args);
    _self->_super.filtrate = _fz_amplifier_filtrate;
    _self->level           = 1.;
    return _self;
}

static
fz_ptr_t
_fz_amplifier_destruct(fz_ptr_t self)
{
    fz_amplifier_t *_self =
            (fz_amplifier_t*) ((fz_object_t*) fz_filter)->destruct(self);
    return _self;
}

static const fz_object_t _fz_amplifier = {
    sizeof (fz_amplifier_t),
    _fz_amplifier_construct,
    _fz_amplifier_destruct,
    NULL,
    NULL
};

const fz_ptr_t fz_amplifier = (const fz_ptr_t) &_fz_amplifier;
