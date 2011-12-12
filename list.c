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
fz_list_destroy(
                fz_list_t **list,
                fz_lirm_f rm_func)
{
    fz_list_t *l = *list;
    fz_uint_t i;
    // free items if function provided
    if (rm_func != NULL) {
        for (i = 0; i < l->size; ++i) {
            rm_func(l->items + (i*l->item_size));
        }
    }
    // free item storage
    if (l->items != NULL) {
        free(l->items);
    }
    // free list storage
    free(l);
    *list = NULL;
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_list_insert(
               fz_list_t    *list, 
               fz_pointer_t item,
               fz_uint_t    pos)
{
    fz_uint_t    size, malloc_size;
    fz_pointer_t items;
    
    if (pos < 0 || pos > list->size) {
        return FZ_RESULT_IOOB_ERROR;
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
fz_list_append(
               fz_list_t    *list,
               fz_pointer_t item)
{
    return fz_list_insert(list, item, list->size);
}