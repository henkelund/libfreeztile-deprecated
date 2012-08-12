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

#include "./string.h"
#include "../freeztile.h"
#include <string.h>
#include <math.h>

static
fz_ptr_t
_fz_string_construct(
                     const fz_ptr_t  self,
                     va_list        *args)
{
    fz_string_t *_self = (fz_string_t*) self;
    fz_string_set(_self, va_arg(*args, const fz_cstr_t));
    return _self;
}

static
fz_ptr_t
_fz_string_destruct(const fz_ptr_t self)
{
    fz_string_t *_self = (fz_string_t*) self;
    fz_string_set(_self, NULL); // Free's _self->cstr if not null
    return _self;
}

static
fz_ptr_t
_fz_string_clone(const fz_ptr_t self)
{
    fz_string_t *_self  = (fz_string_t*) self;
    return fz_new(fz_string, _self->cstr);
}

static
fz_int_t
_fz_string_compare(
                   const fz_ptr_t self,
                   const fz_ptr_t other)
{
    fz_string_t *_self  = (fz_string_t*) self;
    fz_string_t *_other = (fz_string_t*) other;
    if (_self->cstr == _other->cstr) {
        return 0;
    } else if (_self->cstr == NULL) {
        return -1;
    } else if (_other->cstr == NULL) {
        return 1;
    }
    return strcmp(_self->cstr, _other->cstr);
}

static
fz_result_t
_fz_string_grow(
                fz_string_t *self,
                fz_size_t    size)
{
    fz_size_t malloc_size;
    fz_cstr_t cstr;

    size += 1; // make sure there's room for null terminator

    if (size > self->_avail_size) {
        malloc_size = (size < FZ_STRING_MIN_SIZE) ?
                            FZ_STRING_MIN_SIZE    :
                            (fz_size_t) pow(2, ceil(log(size)/log(2)));
        cstr = fz_malloc(malloc_size); // memset's 0
        if (cstr == NULL) {
            return FZ_RESULT_MALLOC_ERROR;
        }
        if (self->cstr != NULL) {
            memcpy(cstr, self->cstr, self->length);
            fz_free(self->cstr);
        }
        self->cstr = cstr;
        self->_avail_size = malloc_size;
    }
    return FZ_RESULT_SUCCESS;
}

static const fz_object_t _fz_string = {
    sizeof (fz_string_t),
    _fz_string_construct,
    _fz_string_destruct,
    _fz_string_clone,
    _fz_string_compare
};

const fz_ptr_t fz_string = (const fz_ptr_t) &_fz_string;

fz_result_t
fz_string_set(
              fz_string_t     *self,
              const fz_cstr_t  cstr)
{
    fz_result_t result;

    if (cstr == NULL) {
        if (self->cstr != NULL) {
            fz_free(self->cstr);
            self->cstr = NULL;
        }
        self->_avail_size = 0;
        self->length      = 0;
        return FZ_RESULT_SUCCESS;
    }

    if (FZ_RESULT_SUCCESS !=
            (result = _fz_string_grow(self, self->length = strlen(cstr)))) {
        return result;
    }
    strcpy(self->cstr, cstr);

    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_string_concat(
                 fz_string_t     *self,
                 const fz_cstr_t  cstr)
{
    fz_result_t result;
    fz_size_t   length;

    if (cstr == NULL || self->cstr == NULL) {
        return fz_string_set(self, cstr);
    }

    length = self->length + strlen(cstr);
    if (FZ_RESULT_SUCCESS != (result = _fz_string_grow(self, length))) {
        return result;
    }
    strcpy(self->cstr + self->length, cstr);
    self->length = length;

    return FZ_RESULT_SUCCESS;
}
