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

#ifndef _FZ_MAP_H_
#define _FZ_MAP_H_ 1

#include "../freeztile.h"
#include "list.h"

BEGIN_C_DECLS

#ifndef FZ_HASH_PRIME
#define FZ_HASH_PRIME 193
#endif

#define FZ_MAP_FLAG_NONE    0
#define FZ_MAP_FLAG_RETAIN (1 << 0)

/**
 * Macro for map creation
 *
 * @param  type       type of map
 * @param  fz_flags_t options
 * @return fz_ptr_t
 */
#define fz_map_new_flags(type, flags) \
            fz_new(fz_map, sizeof (type), #type, flags)

/**
 * Macro for map creation
 *
 * @param  type     type of map
 * @return fz_ptr_t
 */
#define fz_map_new(type) \
            fz_map_new_flags(type, FZ_MAP_FLAG_NONE)

/**
 * Macro for map value retrieval
 *
 * @param  fz_map_t  *map  Map to fetch from
 * @param  fz_char_t *key  Key of value to fetch
 * @param  type       type Type of value
 * @return type
 */
#define fz_map_val(map, key, type) \
            (*((type*) fz_map_get(map, key)))

/**
 * Macro for map value pointer retrieval
 *
 * @param  fz_map_t  *map  Map to fetch from
 * @param  fz_char_t *key  Key of value to fetch
 * @param  type       type Type of value
 * @return type
 */
#define fz_map_ref(map, key, type) \
            ((type*) fz_map_get(map, key))

/**
 * Macro for map key checking
 *
 * @param  fz_map_t  *map  Map to look in
 * @param  fz_char_t *key  Key to check for
 * @return bool
 */
#define fz_map_has(map, key) \
            (fz_map_get(map, key) != NULL)

typedef struct {
    fz_char_t *key;
    fz_ptr_t   value;
} fz_map_item_t;

typedef struct {
    const fz_ptr_t  _class;
    fz_list_t      *table[FZ_HASH_PRIME];
    fz_uint_t       type_size;
    fz_char_t      *type_name;
    fz_uint_t       size;
    fz_flags_t      flags;
} fz_map_t;

const fz_ptr_t fz_map;

/**
 * Add/update a value to/in the map
 *
 * @param  fz_map_t        *map
 * @param  const fz_char_t *key
 * @param  fz_ptr_t         value
 * @return                        FZ_RESULT_SUCCESS on success
 */
fz_result_t fz_map_set(
                       fz_map_t        *map,
                       const fz_char_t *key,
                       fz_ptr_t         value);

/**
 * Retrieve a value from the map
 *
 * @param  fz_map_t        *map
 * @param  const fz_char_t *key
 * @return                      Pointer to identified value or NULL if not found
 */
fz_ptr_t    fz_map_get(
                       fz_map_t        *map,
                       const fz_char_t *key);

/**
 * Unset a value
 *
 * @param  fz_map_t        *map
 * @param  const fz_char_t *key
 * @return                      FZ_RESULT_SUCCESS on success
 */
fz_result_t fz_map_uns(
                       fz_map_t        *map,
                       const fz_char_t *key);

END_C_DECLS

#endif // _FZ_MAP_H_
