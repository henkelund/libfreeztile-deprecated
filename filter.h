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

#include "types.h"

#define FZ_FILTER_OPT_NONE   0
#define FZ_FILTER_OPT_USEENV (1 << 0)

#define FZ_ENV_ATTACK  0
#define FZ_ENV_DECAY   1
#define FZ_ENV_SUSTAIN 2
#define FZ_ENV_RELEASE 3

/**
 *
 * @param filter
 * @return
 */
fz_result_t fz_filter_create(fz_filter_t **filter);

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
 * @param filter
 * @return
 */
fz_result_t fz_filter_lowpass_create(fz_filter_t **filter);

/**
 *
 * @param filter
 * @param cutoff
 * @return
 */
fz_result_t fz_filter_lowpass_rc_set(
                                     fz_filter_t *filter,
                                     fz_float_t   cutoff);

/**
 *
 * @param filter
 * @param cutoff
 * @return
 */
fz_result_t fz_filter_lowpass_rc_get(
                                     fz_filter_t *filter,
                                     fz_float_t  *cutoff);

/**
 *
 * @param filter
 * @param samples
 * @param envelope
 * @return
 */
fz_uint_t   fz_filter_lowpass_function(
                                       fz_filter_t *filter,
                                       fz_list_t   *samples,
                                       fz_list_t   *envelope);

/*fz_uint_t  fz_filter_equalizer_apply(
                                     fz_filter_t *filter,
                                     fz_list_t   *samples);*/

#endif // _FZ_FILTER_H_