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
#include "list.h"
#include "freeztile.h"

fz_result_t
fz_list_create(
               fz_list_t **list,
               fz_uint_t   type_size,
               fz_char_t  *type_name)
{
    fz_list_t *ls = (fz_list_t*)malloc(sizeof(fz_list_t));
    if (ls == NULL) {
        return FZ_RESULT_MALLOC_ERROR;
    }
    ls->type_name = (fz_char_t*)malloc(sizeof(fz_char_t)*(strlen(type_name) + 1));
    if (ls->type_name == NULL) {
        free(ls);
        return FZ_RESULT_MALLOC_ERROR;
    }
    strcpy(ls->type_name, type_name);
    ls->items      = NULL;
    ls->type_size  = type_size;
    ls->size       = 0;
    ls->avail_size = 0;
    ls->remove     = NULL;
    ls->compare    = NULL;
    *list          = ls;
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_list_destroy(fz_list_t **list)
{
    fz_list_t *ls = *list;
    fz_uint_t i;
    // free items if function provided
    if (ls->remove != NULL) {
        for (i = 0; i < ls->size; ++i) {
            ls->remove(ls->items + (i*ls->type_size));
        }
    }
    // free item storage
    if (ls->items != NULL) {
        free(ls->items);
    }
    free(ls->type_name);
    // free list storage
    free(ls);
    *list = NULL;
    return FZ_RESULT_SUCCESS;
}

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
    fz_uint_t    malloc_size;
    fz_pointer_t items;
    if (min_size > list->avail_size) {
        malloc_size = min_size < 10 ? 10 : 
            (fz_uint_t) pow(2, ceil(log(min_size)/log(2)));
        items = malloc(malloc_size*list->type_size);
        if (items == NULL) {
            return FZ_RESULT_MALLOC_ERROR;
        }
        if (list->items != NULL) {
            memcpy(items, list->items, list->size*list->type_size);
            free(list->items);
        }
        list->items = items;
        list->avail_size = malloc_size;
    }
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_list_insert(
               fz_list_t    *list, 
               fz_pointer_t item,
               fz_uint_t    pos)
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
    
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_list_append(
               fz_list_t    *list,
               fz_pointer_t item)
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