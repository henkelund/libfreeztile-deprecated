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

#include "map.h"
#include <string.h>

typedef struct {
    fz_char_t *key;
    fz_ptr_t   value;
} fz_map_item_t;

struct fz_map_s {
    const fz_ptr_t  _class;
    fz_list_t      *table[FZ_HASH_PRIME];
    fz_uint_t       type_size;
    fz_char_t      *type_name;
    fz_flags_t      flags;
    fz_list_t      *iterator;
};

static
fz_ptr_t
_fz_map_construct(
                  fz_ptr_t  self,
                  va_list  *args)
{
    fz_map_t        *_self     = (fz_map_t*) self;
    fz_uint_t        type_size = va_arg(*args, fz_uint_t),
                     i         = 0;
    const fz_char_t *type_name = va_arg(*args, fz_char_t*);
    fz_flags_t       flags     = va_arg(*args, fz_flags_t);

    _self->type_size = type_size;
    _self->type_name =
            (fz_char_t*) fz_malloc(sizeof(fz_char_t)*(strlen(type_name) + 1));
    _self->flags     = flags;

    if (_self->flags & FZ_MAP_FLAG_ITERABLE) {
        _self->iterator = fz_list_new(fz_map_item_t*);
    } else {
        _self->iterator = NULL;
    }

    strcpy(_self->type_name, type_name);

    for (; i < FZ_HASH_PRIME; ++i) {
        _self->table[i] = NULL;
    }

    return _self;
}

static
fz_ptr_t
_fz_map_destruct(fz_ptr_t self)
{
    fz_map_t  *_self = (fz_map_t*) self;
    fz_ptr_t   value;
    fz_uint_t  i, j;

    for (i = 0; i < FZ_HASH_PRIME; ++i) {
        if (_self->table[i] != NULL) {
            for (j = 0; j < fz_list_size(_self->table[i]); ++j) {
                value = fz_list_ref(_self->table[i], j, fz_map_item_t)->value;
                if (_self->flags & FZ_MAP_FLAG_RETAIN) {
                    fz_release(*((fz_ptr_t*) value));
                }
                fz_free(value); // key is freed along with value
            }
            fz_release(_self->table[i]);
        }
    }

    if (_self->flags & FZ_MAP_FLAG_ITERABLE && _self->iterator != NULL) {
        fz_release(_self->iterator);
    }

    fz_free(_self->type_name);

    return _self;
}

static
fz_uint_t
_fz_map_get_index(const fz_char_t *key)
{
    return fz_hash(key, strlen(key))%FZ_HASH_PRIME;
}

static const fz_object_t _fz_map = {
    sizeof (fz_map_t),
    _fz_map_construct,
    _fz_map_destruct,
    NULL,
    NULL
};

const fz_ptr_t fz_map = (const fz_ptr_t) &_fz_map;

fz_result_t
fz_map_set(
           fz_map_t        *map,
           const fz_char_t *key,
           fz_ptr_t         value)
{
    fz_uint_t      i    = _fz_map_get_index(key);
    fz_list_t     *row  = NULL;
    fz_map_item_t *cell = NULL;

    if (map->table[i] == NULL) {
        map->table[i] = fz_list_new(fz_map_item_t);
    }
    row = map->table[i];

    for (i = 0; i < fz_list_size(row); ++i) {
        if (strcmp(key, fz_list_ref(row, i, fz_map_item_t)->key) == 0) {
            cell = fz_list_ref(row, i, fz_map_item_t);
            if (map->flags & FZ_MAP_FLAG_RETAIN) {
                fz_release(*((fz_ptr_t*) cell->value));
                *((fz_ptr_t*) cell->value) = NULL;
            }
            break;
        }
    }

    if (cell == NULL) { // No value found for given key -> allocate new container
        fz_map_item_t new_cell = {.key = NULL, .value = NULL};
        fz_list_append(row, &new_cell);
        cell        = fz_list_ref(row, fz_list_size(row) - 1, fz_map_item_t);
        cell->value = fz_malloc(map->type_size + sizeof (key) + 1);
        cell->key   = ((fz_char_t*) cell->value) + map->type_size;
        strcpy(cell->key, key);
        if (map->flags & FZ_MAP_FLAG_ITERABLE && map->iterator != NULL) {
            fz_list_append(map->iterator, &cell);
        }
    }

    memcpy(cell->value, value, map->type_size);

    if (map->flags & FZ_MAP_FLAG_RETAIN) {
        fz_retain(*((fz_ptr_t*) cell->value));
    }

    return FZ_RESULT_SUCCESS;
}

fz_ptr_t
fz_map_get(
           fz_map_t        *map,
           const fz_char_t *key)
{
    fz_uint_t  i   = _fz_map_get_index(key);
    fz_list_t *row = NULL;

    if (map->table[i] == NULL) {
        return NULL;
    }
    row = map->table[i];

    for (i = 0; i < fz_list_size(row); ++i) {
        if (strcmp(key, fz_list_ref(row, i, fz_map_item_t)->key) == 0) {
            return fz_list_ref(row, i, fz_map_item_t)->value;
        }
    }

    return NULL;
}

fz_result_t
fz_map_uns(
           fz_map_t        *map,
           const fz_char_t *key)
{
    fz_uint_t  i   = _fz_map_get_index(key),
               j;
    fz_list_t *row = NULL;

    if (map->table[i] == NULL) {
        return FZ_RESULT_SUCCESS;
    }
    row = map->table[i];

    for (i = 0; i < fz_list_size(row); ++i) {
        if (strcmp(key, fz_list_ref(row, i, fz_map_item_t)->key) == 0) {
            if (map->flags & FZ_MAP_FLAG_RETAIN) {
                fz_release(*((fz_ptr_t*) fz_list_ref(row, i, fz_map_item_t)->value));
            }
            fz_free(fz_list_ref(row, i, fz_map_item_t)->value);
            // key is freed along with value
            fz_list_remove(row, i);
            if (map->flags & FZ_MAP_FLAG_ITERABLE && map->iterator != NULL) {
                for (j = 0; j < fz_list_size(map->iterator); ++j) {
                    if (strcmp(key, fz_list_val(
                                map->iterator, j, fz_map_item_t*)->key) == 0) {
                        fz_list_remove(map->iterator, j);
                        break;
                    }
                }
            }
            return FZ_RESULT_SUCCESS;
        }
    }

    return FZ_RESULT_SUCCESS;
}

fz_uint_t
fz_map_size(fz_map_t *map)
{
    return map->iterator == NULL ? 0 : fz_list_size(map->iterator);
}

fz_ptr_t
fz_map_iget(
            fz_map_t  *map,
            fz_uint_t  i)
{
    return map->iterator == NULL ?
                NULL : fz_list_val(map->iterator, i, fz_map_item_t*)->value;
}

const fz_char_t*
fz_map_ikey(
            fz_map_t  *map,
            fz_uint_t  i)
{
    return map->iterator == NULL ?
                NULL : fz_list_val(map->iterator, i, fz_map_item_t*)->key;
}

fz_flags_t
fz_map_flags(fz_map_t *map)
{
    return map->flags;
}

fz_result_t
fz_map_set_flags(
                 fz_map_t   *map,
                 fz_flags_t  flags)
{
    map->flags = flags;
    return FZ_RESULT_SUCCESS;
}
