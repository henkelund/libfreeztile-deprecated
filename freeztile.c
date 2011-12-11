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
#include <pthread.h>
#include <errno.h>
#include "freeztile.h"

fz_result_t
fz_list_create(
               fz_list_t **list,
               fz_uint_t item_size)
{
    fz_list_t *l = malloc(sizeof(fz_list_t));
    if (l == NULL) {
        return FZ_RESULT_MALLOC_ERROR;
    }
    l->items = NULL;
    l->item_size = item_size;
    l->size = 0;
    l->avail_size = 0;
    *list = l;
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_list_add(
            fz_list_t    *list, 
            fz_pointer_t item,
            ...)
{
    int          pos;
    fz_uint_t    size, malloc_size;
    fz_pointer_t items;
    
    va_list opt;
    va_start(opt, item);
    pos = va_arg(opt, int);
    va_end(opt);
    if (pos < 0 || pos > list->size) {
        pos = list->size;
    }
    size = list->size + 1;
    
    if (size > list->avail_size) {
        malloc_size = size < 8 ? 8 : (fz_uint_t) pow(2, ceil(log(size)/log(2)));
        items = malloc(malloc_size*list->item_size);
        if (items == NULL) {
            return FZ_RESULT_MALLOC_ERROR;
        }
        if (list->items != NULL) {
            memcpy(items, list->items, list->size*list->item_size);
            free(list->items);
        }
        list->items = items;
        list->avail_size = malloc_size;
    }

    if (pos != list->size) {
        memmove(
            list->items + ((pos + 1)*list->item_size),
            list->items + (pos*list->item_size),
            (list->size - pos)*list->item_size
        );
    }
    
    memcpy(list->items + (pos*list->item_size), item, list->item_size);
    list->size = size;
    
    return FZ_RESULT_SUCCESS;
}

fz_result_t 
fz_lock_create(fz_lock_t **lock)
{
    fz_lock_t *mutex = malloc(sizeof(fz_lock_t));
    if (mutex == NULL) {
        return FZ_RESULT_MALLOC_ERROR;
    }
    if (pthread_mutex_init(mutex, NULL) != 0) {
        free(mutex);
        return FZ_RESULT_LOCK_ERROR;
    }
    *lock = mutex;
    return FZ_RESULT_SUCCESS;
}

fz_result_t 
fz_lock_destroy(fz_lock_t **lock)
{
    if (pthread_mutex_destroy(*lock) != 0 && errno != EINVAL) {
        return FZ_RESULT_LOCK_ERROR;
    }
    free(*lock);
    *lock = NULL;
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_lock_acquire(fz_lock_t *lock)
{
    fz_result_t result;
    if ((result = pthread_mutex_lock(lock)) != 0) {
        return FZ_RESULT_LOCK_ERROR;
    }
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_lock_release(fz_lock_t *lock)
{
    fz_result_t result;
    if ((result = pthread_mutex_unlock(lock)) != 0) {
        return FZ_RESULT_LOCK_ERROR;
    }
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_splbuf_create(
                 fz_splbuf_t **buffer,
                 fz_uint_t   size)
{
    fz_result_t result;
    fz_splbuf_t *buf = malloc(sizeof(fz_splbuf_t));
    if (buf == NULL) {
        return FZ_RESULT_MALLOC_ERROR;
    }
    buf->instants = NULL;
    buf->values = NULL;
    buf->size = 0;
    buf->avail_size = 0;
    if ((result = fz_lock_create(&buf->lock)) != 0) {
        free(buf);
        return result;
    }
    if (size > 0 && (result = fz_splbuf_resize(buf, size)) != 0) {
        free(buf);
        return result;
    }
    *buffer = buf;
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_splbuf_destroy(fz_splbuf_t **buffer)
{
    fz_splbuf_t *buf = *buffer;
    while (fz_lock_destroy(&buf->lock) != 0)
        ; // spin while lock is busy
    
    if (buf->instants != NULL) {
        free(buf->instants);
    }
    if (buf->values != NULL) {
        free(buf->values);
    }
    free(buf);
    *buffer = NULL;
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_splbuf_resize(
                 fz_splbuf_t *buffer,
                 fz_uint_t   size)
{
    // declare variables
    fz_uint_t malloc_size, i;
    fz_splins_t *new_instants;
    fz_splval_t *new_values;

    // request access to the buffer
    fz_lock_acquire(buffer->lock);

    if (size <= buffer->avail_size) {
        // requested size already available
        // just update the public value
        buffer->size = size;

    } else { 
        // requested size not availble -> allocate
        // expand the requested size to the nearest (larger) power of two
        // this way we will have a margin and might avoid resizing to often
        malloc_size = (fz_uint_t) pow(2, ceil(log(size)/log(2)));
        new_instants = malloc(malloc_size*sizeof(fz_splins_t));
        memset(new_instants, 0, malloc_size*sizeof(fz_splins_t));
        new_values = malloc(malloc_size*sizeof(fz_splval_t));
        memset(new_values, 0, malloc_size*sizeof(fz_splval_t));
    
        // transfer old values 
        // this might fail if the given buffer struct is not
        // initialized properly.
        if (buffer->instants != NULL && buffer->values != NULL) {
            for (i = 0; i < buffer->size; ++i) {
                new_instants[i] = buffer->instants[i];
                new_values[i] = buffer->values[i];
            }
            free(buffer->instants);
            free(buffer->values);
        }
    
        // update buffer variables
        buffer->instants = new_instants;
        buffer->values = new_values;
        buffer->avail_size = malloc_size;
        buffer->size = size;
    }

    fz_lock_release(buffer->lock);
    return FZ_RESULT_SUCCESS;
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
    } else if (result == FZ_RESULT_LOCK_ERROR) {
        return "lock error";
    }
    return "unknown result";
}
