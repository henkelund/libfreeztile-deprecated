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

#ifndef _FZ_TYPES_H_
#define _FZ_TYPES_H_

#include <stdlib.h>
#include <stdarg.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef float             fz_float_t;

typedef float             fz_real_t;

typedef int               fz_int_t;

typedef unsigned int      fz_uint_t;

typedef unsigned long int fz_ulong_t;

typedef char              fz_char_t;

typedef int               fz_result_t;

typedef void*             fz_ptr_t;

typedef size_t            fz_size_t;

typedef fz_ulong_t        fz_flags_t;

typedef fz_real_t         fz_amp_t;

typedef fz_real_t         fz_frame_t;

typedef char              fz_bool_t;

// fz base class
typedef struct {
    fz_size_t   size;
    fz_ptr_t  (*construct) (fz_ptr_t       self, va_list *args);
    fz_ptr_t  (*destruct)  (fz_ptr_t       self);
    fz_ptr_t  (*clone)     (const fz_ptr_t self);
    fz_int_t  (*compare)   (const fz_ptr_t self, const fz_ptr_t other);
} fz_object_t;

#endif // _FZ_TYPES_H_
