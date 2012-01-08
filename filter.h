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

#ifndef _FZ_FILTER_H_
#define _FZ_FILTER_H_

#include <stdarg.h>
#include "types.h"

#define FZ_FILTER_OPT_NONE   0
#define FZ_FILTER_OPT_USEENV (1 << 0)

#define FZ_ENV_ATTACK  0
#define FZ_ENV_DECAY   1
#define FZ_ENV_SUSTAIN 2
#define FZ_ENV_RELEASE 3

/**
 * 
 * @param self
 * @param args
 * @return 
 */
fz_ptr_t    fz_filter_construct(
                             const fz_ptr_t  self,
                             va_list        *args);

/**
 *
 * @param filter
 * @param samples
 * @return
 */
fz_uint_t   fz_filter_apply(
                            fz_filter_t *filter, 
                            fz_list_t   *samples);

/**
 * 
 * @param self
 * @param args
 * @return 
 */
fz_ptr_t    fz_lowpass_construct(
                              const fz_ptr_t  self,
                              va_list        *args);

/**
 *
 * @param filter
 * @param samples
 * @param envelope
 * @return
 */
fz_uint_t   fz_lowpass_filter(
                              fz_ptr_t   filter,
                              fz_list_t *samples);

// static filter object descriptor
static const fz_object_t _fz_filter = {
    sizeof (fz_filter_t),
    fz_filter_construct,
    NULL,
    NULL,
    NULL
};

const fz_ptr_t fz_filter = (const fz_ptr_t) &_fz_filter;

static const fz_object_t _fz_lowpass = {
    sizeof (fz_lowpass_t),
    fz_lowpass_construct,
    NULL,
    NULL,
    NULL
};

const fz_ptr_t fz_lowpass = (const fz_ptr_t) &_fz_lowpass;

#endif // _FZ_FILTER_H_