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
#include <math.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <inttypes.h>
#include "freeztile.h"

#if defined FZ_DEBUG && FZ_DEBUG == 1
typedef struct {
    fz_uint_t id_counter;
    fz_size_t malloc_count;
    fz_size_t malloc_size;
} fz_meminfo_t;

static fz_meminfo_t _fz_meminfo = {
  .id_counter   = 0,
  .malloc_count = 0,
  .malloc_size  = 0
};

typedef struct {
    fz_uint_t id;
    fz_size_t size;
} fz_malloc_meta_t;
#endif // defined FZ_DEBUG && FZ_DEBUG == 1

fz_ptr_t
fz_malloc(fz_size_t size)
{
#if defined FZ_DEBUG && FZ_DEBUG == 1
    fz_malloc_meta_t *meta =
            (fz_malloc_meta_t*) malloc(sizeof (fz_malloc_meta_t) + size);
    assert(meta);
    meta->id   = 0;
    meta->size = sizeof (fz_malloc_meta_t) + size;
    meta->id   = _fz_meminfo.id_counter++;
    ++_fz_meminfo.malloc_count;
    _fz_meminfo.malloc_size += meta->size;
    fzdebug(
            "\t+ #%u (%uB) - %upc (%uB)\n",
            meta->id,
            (unsigned int) meta->size,
            (unsigned int) _fz_meminfo.malloc_count,
            (unsigned int) _fz_meminfo.malloc_size);
    return (fz_ptr_t) (meta + 1);
#else
    fz_ptr_t ptr = (fz_ptr_t) malloc(size);
    assert(ptr);
    return ptr;
#endif // defined FZ_DEBUG && FZ_DEBUG == 1
}

void
fz_free(fz_ptr_t ptr)
{
#if defined FZ_DEBUG && FZ_DEBUG == 1
    fz_malloc_meta_t *meta = ((fz_malloc_meta_t*) ptr) - 1;
    --_fz_meminfo.malloc_count;
    _fz_meminfo.malloc_size -= meta->size;
    fzdebug(
            "\t- #%u (%uB) - %upc (%uB)\n",
            meta->id,
            (unsigned int) meta->size,
            (unsigned int) _fz_meminfo.malloc_count,
            (unsigned int) _fz_meminfo.malloc_size);
    free(meta);
#else
    free(ptr);
#endif // defined FZ_DEBUG && FZ_DEBUG == 1
}

fz_ptr_t
fz_new(const fz_ptr_t type, ...)
{
    const fz_object_t *_type  = (const fz_object_t*) type;
    fz_int_t          *refcnt =
                            (fz_int_t*) fz_malloc(sizeof (fz_int_t) + _type->size);
    fz_ptr_t           self   = (fz_ptr_t) (refcnt + 1);

    *refcnt = 1;

    // put a type ref at top of self
    *((const fz_object_t**) self) = type;
    if (_type->construct) {
        va_list ap;
        va_start(ap, type);
        self = _type->construct(self, &ap);
        va_end(ap);
    }
    return self;
}

fz_ptr_t
fz_retain(fz_ptr_t obj)
{
    ++(*(((fz_int_t*) obj) - 1));
    return obj;
}

void
fz_release(fz_ptr_t self)
{
    fz_int_t *refcnt = ((fz_int_t*) self) - 1;

    if (--(*refcnt) <= 0) {
        // type pointer is at top of object, @see fz_new
        const fz_object_t **type = self;
        if (self && *type && (*type)->destruct) {
            self = (*type)->destruct(self);
        }
        fz_free(refcnt);
    }
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

const fz_char_t*
fz_result_string(fz_result_t result)
{
    switch (result) {
        case FZ_RESULT_SUCCESS:
            return "success";
            break;
        case FZ_RESULT_NOT_IMPLEMENTED:
            return "not implemented";
            break;
        case FZ_RESULT_MALLOC_ERROR:
            return "memory allocation error";
            break;
        case FZ_RESULT_IOOB_ERROR:
            return "index out-of-bounds error";
            break;
        case FZ_RESULT_INVALID_ARG:
            return "invalid argument";
            break;
        default:
            return "unknown result";
            break;
    }
}
