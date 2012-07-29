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

#ifndef _FZ_FREEZTILE_H_
#define _FZ_FREEZTILE_H_

#include <stdarg.h>
#include <inttypes.h>
#include "types.h"

//#define FZ_DEBUG 1

#if defined FZ_DEBUG && FZ_DEBUG == 1
    #include <stdio.h>
    #define FZ_IF_DEBUG(code) code
#else
    #define FZ_IF_DEBUG(code) /* code */
#endif

/**
 * FreeZtile return values
 *
 */
#define FZ_RESULT_SUCCESS          0
#define FZ_RESULT_NOT_IMPLEMENTED -1
#define FZ_RESULT_MALLOC_ERROR    -2
#define FZ_RESULT_IOOB_ERROR      -3
#define FZ_RESULT_INVALID_ARG     -4

/**
 *
 * @param  fz_ptr_t type
 * @param  ...
 * @return fz_ptr_t New object of given type
 */
fz_ptr_t    fz_new(const fz_ptr_t type, ...);

/**
 * Increase reference count of obj
 *
 * @param  fz_ptr_t obj
 * @return fz_ptr_t obj
 */
fz_ptr_t    fz_retain(fz_ptr_t obj);

/**
 *
 * @param  fz_ptr_t self
 */
void        fz_free(fz_ptr_t self);

/**
 * Returns a DJBX33X hash integer of the given data
 *
 * @param  data
 * @param  size
 * @return
 */
uint32_t    fz_hash(
                    const fz_char_t *data,
                    fz_size_t        size);

/**
 *
 * @param  fz_result_t result
 * @return const char*
 */
const char* fz_result_string(fz_result_t result);

#endif // _FZ_FREEZTILE_H_
