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

#ifndef _FZ_STRING_H_
#define _FZ_STRING_H_

#include "../freeztile.h"

BEGIN_C_DECLS

#ifndef FZ_STRING_MIN_SIZE
#define FZ_STRING_MIN_SIZE 8
#endif

typedef char* fz_cstr_t;

typedef struct {
    const fz_ptr_t _class;
    fz_size_t      _avail_size;
    fz_cstr_t      cstr;
    fz_size_t      length;
} fz_string_t;

/**
 *
 * @param  fz_string_t *   dest
 * @param  const fz_cstr_t source
 * @return fz_result_t
 */
fz_result_t     fz_string_set PARAMS((fz_string_t *, const fz_cstr_t));

/**
 *
 * @param  fz_string_t *
 * @param  const fz_cstr_t
 * @return fz_result_t
 */
fz_result_t     fz_string_concat PARAMS((fz_string_t *, const fz_cstr_t));

const fz_ptr_t fz_string;

END_C_DECLS

#endif // _FZ_STRING_H_
