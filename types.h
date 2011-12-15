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

#ifndef _FZ_TYPES_H_
#define _FZ_TYPES_H_

#include <pthread.h>

typedef float           fz_float_t;

typedef int             fz_int_t;

typedef unsigned int    fz_uint_t;

typedef int             fz_result_t;

typedef void*           fz_pointer_t;

// sample value (amplitude)
typedef fz_float_t      fz_splval_t;

// sample instant
typedef fz_float_t      fz_splins_t;

typedef pthread_mutex_t fz_lock_t;

// fz_list_t function types
typedef void            (*fz_lirm_f)(void *item);
typedef fz_int_t        (*fz_licmp_f)(void *a, void *b);

typedef struct {
    fz_float_t x;
    fz_float_t y;
} fz_pointf_t;

typedef struct {
    fz_pointer_t items;
    fz_uint_t    item_size;
    fz_uint_t    size;
    fz_uint_t    avail_size;
    fz_lirm_f    remove;
    fz_licmp_f   compare;
} fz_list_t;

typedef struct {
    fz_splins_t instant;
    fz_splval_t value;
} fz_sample_t;

typedef struct {
    fz_uint_t   state;
    fz_splval_t *prototype;
    fz_uint_t   proto_size;
    fz_uint_t   proto_avail_size;
    fz_uint_t   version;
} fz_form_t;

typedef struct {
    fz_form_t   *form;
    fz_splins_t instant;
    fz_splval_t amplitude;
    fz_float_t  frequency;
    fz_float_t  sample_rate;
} fz_oscillator_t;

typedef struct {
    fz_pointf_t start;
    fz_pointf_t end;
    fz_pointf_t a;
    fz_pointf_t b;
    fz_float_t  tolerance;
    fz_uint_t   version;
} fz_curve_t;

typedef struct {
    fz_list_t   *forms;
    fz_list_t   *curves;
    fz_list_t   *proprtions;
} fz_multicurve_t;

#endif // _FZ_TYPES_H_
