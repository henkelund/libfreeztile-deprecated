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
#include "list.h"
#include "../freeztile.h"

// ### PRIVATE ###

static
fz_ptr_t
_fz_list_construct(
                  const fz_ptr_t  self,
                  va_list        *args)
{
    fz_list_t       *_self     = (fz_list_t*) self;
    fz_uint_t        type_size = va_arg(*args, fz_uint_t);
    const fz_char_t *type_name = va_arg(*args, fz_char_t*);
    fz_flags_t       flags     = va_arg(*args, fz_flags_t);

    _self->type_name =
            (fz_char_t*) fz_malloc(sizeof(fz_char_t)*(strlen(type_name) + 1));
    strcpy(_self->type_name, type_name);
    _self->items      = NULL;
    _self->type_size  = type_size;
    _self->size       = 0;
    _self->avail_size = 0;
    _self->flags      = flags;
    _self->remove     = NULL;
    _self->compare    = NULL;
    return _self;
}

static
fz_ptr_t
_fz_list_destruct(fz_ptr_t self)
{
    fz_list_t *_self = (fz_list_t*) self;
    fz_uint_t i;
    if (_self->remove != NULL) {
        for (i = 0; i < _self->size; ++i) {
            _self->remove(_self->items + (i*_self->type_size));
        }
    } else if (_self->flags & FZ_LIST_FLAG_RETAIN) {
        for (i = 0; i < _self->size; ++i) {
            fz_release(fz_list_val(_self, i, fz_ptr_t));
        }
    }
    // free item storage
    if (_self->items != NULL) {
        fz_free(_self->items);
    }
    fz_free(_self->type_name);
    _self->type_name = NULL;
    return _self;
}

static
fz_uint_t
_fz_list_quicksort_partition(
                             fz_list_t *self,
                             fz_int_t   left_index,
                             fz_int_t   right_index,
                             fz_int_t   pivot_index,
                             fz_int_t   dir)
{
    fz_int_t  i,
              store_index = left_index;
    fz_char_t pivot[self->type_size];
    fz_ptr_t  other;

    // Store pivot value & move it to the end
    memcpy(pivot, self->items + (pivot_index*self->type_size), self->type_size);
    fz_list_swap(self, pivot_index, right_index);

    for (i = left_index; i < right_index; ++i) {
        other = self->items + (i*self->type_size);
        if ((self->compare == NULL ?
                // Assume object ptr if no cmp fnc, will probably crash if not so
                dir*fz_compare(*((fz_ptr_t*) other), *((fz_ptr_t*) pivot)) :
                dir*self->compare(other, pivot)) < 0) {
            fz_list_swap(self, i, store_index);
            ++store_index;
        }
    }

    fz_list_swap(self, store_index, right_index);

    return store_index;
}

static
fz_result_t
_fz_list_quicksort(
                   fz_list_t *self,
                   fz_int_t   left_index,
                   fz_int_t   right_index,
                   fz_int_t   dir)
{
    fz_int_t pivot_index;

    if (left_index < right_index) {

        pivot_index =
                _fz_list_quicksort_partition(self, left_index, right_index,
                        // Less likely to overflow than (left_index + right_index)/2
                        left_index + (right_index - left_index)/2,
                        dir);

        _fz_list_quicksort(self, left_index,      pivot_index - 1, dir);
        _fz_list_quicksort(self, pivot_index + 1, right_index,     dir);
    }
    return FZ_RESULT_SUCCESS;
}

static const fz_object_t _fz_list = {
    sizeof (fz_list_t),
    _fz_list_construct,
    _fz_list_destruct,
    NULL,
    NULL
};

// ### PUBLIC ###

const fz_ptr_t fz_list = (const fz_ptr_t) &_fz_list;

fz_result_t
fz_list_clear(
              fz_list_t *list,
              fz_uint_t size)
{
    fz_result_t result;
    fz_uint_t   i;
    if ((result = fz_list_grow(list, size)) != FZ_RESULT_SUCCESS) {
        return result;
    }
    if (list->remove != NULL) {
        for (i = 0; i < list->size; ++i) {
            list->remove(list->items + (i*list->type_size));
        }
    } else if (list->flags & FZ_LIST_FLAG_RETAIN) {
        for (i = 0; i < list->size; ++i) {
            fz_release(fz_list_val(list, i, fz_ptr_t));
        }
    }
    list->size = size;
    memset(list->items, 0, list->type_size*list->size);
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_list_grow(
               fz_list_t *list,
               fz_uint_t min_size)
{
    fz_uint_t malloc_size;
    fz_ptr_t  items;
    if (min_size > list->avail_size) {
        malloc_size = min_size < 10 ? 10 :
            (fz_uint_t) pow(2, ceil(log(min_size)/log(2)));
        items = fz_malloc(malloc_size*list->type_size);
        if (items == NULL) {
            return FZ_RESULT_MALLOC_ERROR;
        }
        if (list->items != NULL) {
            memcpy(items, list->items, list->size*list->type_size);
            fz_free(list->items);
        }
        list->items = items;
        list->avail_size = malloc_size;
    }
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_list_insert(
               fz_list_t *list,
               fz_ptr_t   item,
               fz_uint_t  pos)
{
    fz_uint_t   size;
    fz_result_t result;

    if (pos < 0 || pos > list->size) {
        return FZ_RESULT_IOOB_ERROR;
    }
    size = list->size + 1;
    if ((result = fz_list_grow(list, size)) != FZ_RESULT_SUCCESS) {
        return result;
    }

    if (pos != list->size) {
        memmove(
            list->items + ((pos + 1)*list->type_size),
            list->items + (pos*list->type_size),
            (list->size - pos)*list->type_size
        );
    }

    memcpy(list->items + (pos*list->type_size), item, list->type_size);
    list->size = size;

    if (list->flags & FZ_LIST_FLAG_RETAIN) {
        fz_retain(fz_list_val(list, pos, fz_ptr_t));
    }

    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_list_append(
               fz_list_t *list,
               fz_ptr_t   item)
{
    return fz_list_insert(list, item, list->size);
}

fz_result_t
fz_list_remove(
               fz_list_t *list,
               fz_uint_t pos)
{
    if (pos < 0 || pos >= list->size) {
        return FZ_RESULT_IOOB_ERROR;
    }
    if (list->remove != NULL) {
        list->remove(list->items + (pos*list->type_size));
    } else if (list->flags & FZ_LIST_FLAG_RETAIN) {
        fz_release(fz_list_val(list, pos, fz_ptr_t));
    }
    --list->size;
    if (pos != list->size) {
        memmove(
            list->items + (pos*list->type_size),
            list->items + ((pos + 1)*list->type_size),
            (list->size - pos)*list->type_size
        );
    }
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_list_swap(
             fz_list_t *list,
             fz_uint_t  i1,
             fz_uint_t  i2)
{
    fz_char_t tmp[list == NULL ? 0 : list->type_size];
    fz_ptr_t  p1,
              p2;
    if (list == NULL) {
        return FZ_RESULT_INVALID_ARG;
    } else if (i1 < 0 || i1 >= list->size || i2 < 0 || i2 >= list->size) {
        return FZ_RESULT_IOOB_ERROR;
    }

    if (i1 != i2) {
        p1 = list->items + (i1*list->type_size);
        p2 = list->items + (i2*list->type_size);
        memmove(tmp, p1,  list->type_size);
        memmove(p1,  p2,  list->type_size);
        memmove(p2,  tmp, list->type_size);
    }

    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_list_concat(
               fz_list_t *self,
               fz_list_t *other)
{
    fz_size_t i = 0, osize;
    if (self == NULL || other == NULL || self->type_size != other->type_size) {
        return FZ_RESULT_INVALID_ARG;
    }
    fz_list_grow(self, self->size + other->size);
    osize = other->size; // in case self == other
    for (; i < osize; ++i) {
        fz_list_append(self, other->items + (i*other->type_size));
    }
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_list_sort(fz_list_t *self)
{
    return _fz_list_quicksort(self, 0, self->size - 1, 1);
}

fz_result_t
fz_list_rsort(fz_list_t *self)
{
    return _fz_list_quicksort(self, 0, self->size - 1, -1);
}
