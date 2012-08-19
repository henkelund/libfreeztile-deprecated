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

#ifndef _FZ_LIST_H_
#define _FZ_LIST_H_

#include <string.h>
#include "../types.h"

#define FZ_LIST_FLAG_NONE    0
#define FZ_LIST_FLAG_RETAIN (1 << 0)

typedef struct {
    const fz_ptr_t   _scp; // static class pointer
    fz_ptr_t         items;
    fz_uint_t        type_size;
    fz_char_t       *type_name;
    fz_uint_t        size;
    fz_uint_t        avail_size;
    fz_flags_t       flags;
    fz_result_t    (*remove) (fz_ptr_t item);
    fz_int_t       (*compare)(const fz_ptr_t a, const fz_ptr_t b);
} fz_list_t;

/**
 * Convenience macro for list creation
 *
 * @param  type       type of list
 * @param  fz_flags_t options
 * @return fz_ptr_t
 */
#define fz_list_new_flags(type, flags) \
            fz_new(fz_list, sizeof (type), #type, flags)

/**
 * Convenience macro for list creation
 *
 * @param  type     type of list
 * @return fz_ptr_t
 */
#define fz_list_new(type) \
            fz_list_new_flags(type, FZ_LIST_FLAG_NONE)

/**
 * Convenience macro for list item retrieval
 *
 * @param  fz_list_t* list List to fetch from
 * @param  fz_uint_t  i    Index of item to fetch
 * @param  type       type Type of item
 * @return type
 */
#define fz_list_val(list, i, type) \
            (*(((type*)list->items) + i))

/**
 * Convenience macro for list item pointer retrieval
 *
 * @param  fz_list_t* list List to fetch from
 * @param  fz_uint_t  i    Index of item to fetch
 * @param  type       type Type of item
 * @return *type
 */
#define fz_list_ref(list, i, type) \
            (((type*)list->items) + i)

/**
 * Convenience macro for list type checking
 *
 * @param  fz_list_t* list
 * @param  type       type
 * @return bool       True if type matches
 */
#define fz_list_type(list, type) \
            (list->type_size == sizeof(type) && strcmp(list->type_name, #type) == 0)

/**
 * Helper macro for giving item ownership to retaining object type lists
 *
 * @param  list
 * @param  item
 * @param  pos
 * @param  capture variable name
 * @param  type
 * @return
 */
#define fz_list_insert_release(list, item, pos, capture, type) \
            type capture = item; fz_uint_t capture ## _pos = pos; \
            capture = (fz_list_insert(list, &capture, pos) == FZ_RESULT_SUCCESS ? \
                (fz_release(capture), fz_list_val(list, capture ## _pos, type)) : NULL)

/**
 * Helper macro for giving item ownership to retaining object type lists
 *
 * @param  list
 * @param  item
 * @param  capture variable name
 * @param  type
 * @return
 */
#define fz_list_append_release(list, item, capture, type) \
            type capture = item; \
            capture = (fz_list_insert(list, &capture, list->size) == FZ_RESULT_SUCCESS ? \
                (fz_release(capture), fz_list_val(list, list->size - 1, type)) : NULL)

/**
 * Create a compare callback function for a primitive type
 *
 * @param name
 * @param type
 */
#define FZ_LIST_CMP_FNC(name, type) \
            fz_int_t name(const fz_ptr_t a, const fz_ptr_t b) \
            { return *((type*) a) - *((type*) b); }

/**
 *
 * @param  fz_list_t *list
 * @param  fz_uint_t min_size
 * @return fz_result
 */
fz_result_t fz_list_grow(
                         fz_list_t *list,
                         fz_uint_t  min_size);

/**
 *
 * @param  fz_list_t *list
 * @param  fz_uint_t size
 * @return fz_result
 */
fz_result_t fz_list_clear(
                          fz_list_t *list,
                          fz_uint_t  size);

/**
 *
 * @param  fz_list_t   *list
 * @param  fz_ptr_t     item
 * @param  fz_uint_t    pos
 * @return fz_result_t
 */
fz_result_t fz_list_insert(
                           fz_list_t *list,
                           fz_ptr_t   item,
                           fz_uint_t  pos);

/**
 *
 * @param  fz_list_t   *list
 * @param  fz_ptr_t     item
 * @return fz_result_t
 */
fz_result_t fz_list_append(
                           fz_list_t *list,
                           fz_ptr_t   item);

/**
 *
 * @param  fz_list_t    *list
 * @param  fz_uint_t    pos
 * @return fz_result_t
 */
fz_result_t fz_list_remove(
                           fz_list_t *list,
                           fz_uint_t  pos);

/**
 *
 * @param  fz_list_t   *list
 * @param  fz_uint_t    i1
 * @param  fz_uint_t    i2
 * @return fz_result_t
 */
fz_result_t fz_list_swap(
                         fz_list_t *list,
                         fz_uint_t  i1,
                         fz_uint_t  i2);

/**
 *
 * @param  fz_list_t   *self
 * @param  fz_list_t   *other
 * @return fz_result_t
 */
fz_result_t fz_list_concat(
                           fz_list_t *self,
                           fz_list_t *other);

/**
 *
 * @param  fz_list_t   *list
 * @return fz_result_t
 */
fz_result_t fz_list_sort(fz_list_t *list);

/**
 *
 * @param  fz_list_t   *list
 * @return fz_result_t
 */
fz_result_t fz_list_rsort(fz_list_t *list);

const fz_ptr_t fz_list;

#endif // _FZ_LIST_H_
