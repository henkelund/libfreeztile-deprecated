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

#include "filter.h"
#include "../util/list.h"

static
fz_ptr_t
_fz_filter_construct(
                     fz_ptr_t  self,
                     va_list  *args)
{
    (void) args;
    fz_filter_t *_self = (fz_filter_t*) self;
    _self->filtrate    = NULL;
    _self->regulator   = NULL;
    _self->regbuf      = fz_list_new(fz_real_t);
    return _self;
}

static
fz_ptr_t
_fz_filter_destruct(fz_ptr_t self)
{
    fz_filter_t *_self = (fz_filter_t*) self;
    fz_free(_self->regbuf);
    return _self;
}

static const fz_object_t _fz_filter = {
    sizeof (fz_filter_t),
    _fz_filter_construct,
    _fz_filter_destruct,
    NULL,
    NULL
};

const fz_ptr_t fz_filter = (const fz_ptr_t) &_fz_filter;

fz_result_t
fz_filtrate(
            fz_ptr_t   filter,
            fz_list_t *buffer)
{
    fz_filter_t *_filter = (fz_filter_t*) filter;

    if (!_filter || !_filter->filtrate) {
        return -FZ_RESULT_INVALID_ARG;
    }

    if (_filter->regulator) {
        fz_list_clear(_filter->regbuf, buffer->size);
        fz_produce(_filter->regulator, _filter->regbuf);
    }

    return _filter->filtrate(_filter, buffer);
}
