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

#ifndef _FZ_LIST_H_
#define _FZ_LIST_H_

#include "types.h"

/**
 * Convenience macro for list item retrieval
 *
 * @param  fz_list_t* list List to fetch from
 * @param  fz_uint_t  i    Index of item to fetch
 * @param  type       type Type of item
 * @return type
 */
#define FZ_LIST_VAL(list, i, type) \
            (*(((type*)list->items) + i))
/**
 * Convenience macro for list item pointer retrieval
 *
 * @param  fz_list_t* list List to fetch from
 * @param  fz_uint_t  i    Index of item to fetch
 * @param  type       type Type of item
 * @return *type
 */
#define FZ_LIST_REF(list, i, type) \
            (((type*)list->items) + i)

/**
 * 
 * @param list
 * @param item_size
 * @return 
 */
#define FZ_LIST_TYPE(list, type) \
            (list->item_size == sizeof(type))

/**
 * 
 * @param  fz_list_t   **list
 * @param  fz_uint_t   item_size
 * @return fz_result_t
 */
fz_result_t fz_list_create(
                           fz_list_t **list,
                           fz_uint_t item_size);

/**
 * 
 * @param  fz_list_t   **list
 * @return fz_result_t
 */
fz_result_t fz_list_destroy(fz_list_t **list);

/**
 * 
 * @param  fz_list_t *list
 * @param  fz_uint_t min_size
 * @return fz_result
 */
fz_result_t fz_list_grow(
                         fz_list_t *list,
                         fz_uint_t min_size);

/**
 * 
 * @param  fz_list_t *list
 * @param  fz_uint_t size
 * @return fz_result
 */
fz_result_t fz_list_clear(
                          fz_list_t *list,
                          fz_uint_t size);

/**
 * 
 * @param  fz_list_t    *list
 * @param  fz_pointer_t item
 * @param  fz_uint_t    pos
 * @return fz_result_t
 */
fz_result_t fz_list_insert(
                           fz_list_t    *list,
                           fz_pointer_t item,
                           fz_uint_t    pos);

/**
 * 
 * @param  fz_list_t    *list
 * @param  fz_pointer_t item
 * @return fz_result_t
 */
fz_result_t fz_list_append(
                           fz_list_t    *list,
                           fz_pointer_t item);

/**
 * 
 * @param  fz_list_t    *list
 * @param  fz_uint_t    pos
 * @return fz_result_t
 */
fz_result_t fz_list_remove(
                           fz_list_t *list,
                           fz_uint_t pos);

#endif // _FZ_LIST_H_