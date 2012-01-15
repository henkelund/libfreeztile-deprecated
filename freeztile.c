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
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <inttypes.h>
#include "freeztile.h"

#include <stdio.h>
static fz_int_t _fz_obj_count = 0;

fz_ptr_t
fz_new(const fz_ptr_t type, ...)
{
    const fz_object_t* _type = (const fz_object_t*) type;
    fz_ptr_t self = (fz_ptr_t) malloc(_type->size);
    assert(self);
    // put a type ref at top of self
    *((const fz_object_t**) self) = type;
    if (_type->construct) {
        va_list ap;
        va_start(ap, type);
        self = _type->construct(self, &ap);
        va_end(ap);
    }
    printf("%d objects (+)\n", ++_fz_obj_count);
    return self;
}

void
fz_free(fz_ptr_t self)
{
    // type pointer is at top of object, @see fz_new
    const fz_object_t **type = self;
    if (self && *type && (*type)->destruct) {
        self = (*type)->destruct(self);
    }
    free(self);
    printf("%d objects (-)\n", --_fz_obj_count);
}

uint32_t
fz_hash(
        const fz_char_t *data,
        fz_size_t        size)
{
    uint32_t hash = 5381;
    for (; size > 0; --size) {
        hash = ((hash << 5) + hash) ^ *data++;
    }
    return hash;
}

const char*
fz_result_string(fz_result_t result)
{
    if (result == FZ_RESULT_SUCCESS) {
        return "success";
    } else if (result == FZ_RESULT_NOT_IMPLEMENTED) {
        return "not implemented";
    } else if (result == FZ_RESULT_MALLOC_ERROR) {
        return "memory allocation error";
    } else if (result == FZ_RESULT_IOOB_ERROR) {
        return "index out-of-bounds error";
    } else if (result == FZ_RESULT_INVALID_ARG) {
        return "invalid argument";
    }
    return "unknown result";
}
