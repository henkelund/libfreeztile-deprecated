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
#include <stdarg.h>
#include "../types.h"

/**
 * Convenience macro for list creation
 *
 * @param  type     type of list
 * @return fz_ptr_t
 */
#define fz_list_new(type) \
            fz_new(fz_list, sizeof(type), #type)

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

const fz_ptr_t fz_list;

#endif // _FZ_LIST_H_
