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
#include "../freeztile.h"

BEGIN_C_DECLS

#define FZ_LIST_FLAG_NONE    0
#define FZ_LIST_FLAG_RETAIN (1 << 0)

typedef struct fz_list_s fz_list_t;

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
            (*((type*) fz_list_item(list, i)))

/**
 * Convenience macro for list item pointer retrieval
 *
 * @param  fz_list_t* list List to fetch from
 * @param  fz_uint_t  i    Index of item to fetch
 * @param  type       type Type of item
 * @return *type
 */
#define fz_list_ref(list, i, type) \
            ((type*) fz_list_item(list, i))

/**
 * Convenience macro for list type checking
 *
 * @param  fz_list_t* list
 * @param  type       type
 * @return bool       True if type matches
 */
#define fz_list_type(list, type) \
            (fz_list_type_size(list) == sizeof(type) \
                && strcmp(fz_list_type_name(list), #type) == 0)

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
 * @return fz_uint_t
 */
fz_uint_t        fz_list_size PARAMS((fz_list_t *list));

/**
 *
 * @param  fz_list_t *list
 * @param  fz_uint_t  i
 * @return fz_ptr_t
 */
fz_ptr_t         fz_list_item PARAMS((fz_list_t *list, fz_uint_t i));

/**
 *
 * @param  fz_list_t *list
 * @return fz_uint_t
 */
fz_uint_t        fz_list_type_size PARAMS((fz_list_t *list));

/**
 *
 * @param  fz_list_t *list
 * @return const fz_char_t*
 */
const fz_char_t* fz_list_type_name PARAMS((fz_list_t *list));

/**
 *
 * @param  fz_list_t *list
 * @return fz_flags_t
 */
fz_flags_t       fz_list_flags PARAMS((fz_list_t *list));

/**
 *
 * @param  fz_list_t  *list
 * @param  fz_flags_t  flags
 * @return fz_result_t
 */
fz_result_t      fz_list_set_flags PARAMS((
                                           fz_list_t  *list,
                                           fz_flags_t  flags));

/**
 *
 * @param  fz_list_t *list
 * @param  fz_uint_t min_size
 * @return fz_result
 */
fz_result_t      fz_list_grow PARAMS((
                                      fz_list_t *list,
                                      fz_uint_t  min_size));

/**
 *
 * @param  fz_list_t *list
 * @param  fz_uint_t size
 * @return fz_result
 */
fz_result_t      fz_list_clear PARAMS((
                                       fz_list_t *list,
                                       fz_uint_t  size));

/**
 *
 * @param  fz_list_t   *list
 * @param  fz_ptr_t     item
 * @param  fz_uint_t    pos
 * @return fz_result_t
 */
fz_result_t      fz_list_insert PARAMS((
                                        fz_list_t *list,
                                        fz_ptr_t   item,
                                        fz_uint_t  pos));

/**
 *
 * @param  fz_list_t   *list
 * @param  fz_ptr_t     item
 * @return fz_result_t
 */
fz_result_t      fz_list_append PARAMS((
                                        fz_list_t *list,
                                        fz_ptr_t   item));

/**
 *
 * @param  fz_list_t    *list
 * @param  fz_uint_t    pos
 * @return fz_result_t
 */
fz_result_t      fz_list_remove PARAMS((
                                        fz_list_t *list,
                                        fz_uint_t  pos));

/**
 *
 * @param  fz_list_t   *list
 * @param  fz_uint_t    i1
 * @param  fz_uint_t    i2
 * @return fz_result_t
 */
fz_result_t      fz_list_swap PARAMS((
                                      fz_list_t *list,
                                      fz_uint_t  i1,
                                      fz_uint_t  i2));

/**
 *
 * @param  fz_list_t   *self
 * @param  fz_list_t   *other
 * @return fz_result_t
 */
fz_result_t      fz_list_concat PARAMS((
                                        fz_list_t *self,
                                        fz_list_t *other));

/**
 *
 * @param  fz_list_t   *list
 * @param  fz_ptr_t    *cmp
 * @return fz_result_t
 */
fz_result_t      fz_list_set_compare_callback PARAMS((
                                                      fz_list_t *list,
                                                      fz_ptr_t   cmp));

/**
 *
 * @param  fz_list_t   *list
 * @param  fz_ptr_t    *rm
 * @return fz_result_t
 */
fz_result_t      fz_list_set_remove_callback PARAMS((
                                                     fz_list_t *list,
                                                     fz_ptr_t   rm));

/**
 *
 * @param  fz_list_t   *list
 * @return fz_result_t
 */
fz_result_t      fz_list_sort PARAMS((fz_list_t *list));

/**
 *
 * @param  fz_list_t   *list
 * @return fz_result_t
 */
fz_result_t      fz_list_rsort PARAMS((fz_list_t *list));

const fz_ptr_t fz_list;

END_C_DECLS

#endif // _FZ_LIST_H_
