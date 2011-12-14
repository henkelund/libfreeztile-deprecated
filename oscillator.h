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

#ifndef _FZ_OSCILLATOR_H_
#define _FZ_OSCILLATOR_H_

#include "types.h"

/**
 *
 * @param  fz_oscillator_t**
 * @return fz_result_t
 */
fz_result_t fz_oscillator_create(fz_oscillator_t **oscillator);

/**
 *
 * @param  fz_oscillator_t**
 * @return fz_result_t
 */
fz_result_t fz_oscillator_destroy(fz_oscillator_t **oscillator);

/**
 *
 * @param  fz_list_t        *samples
 * @param  fz_oscillator_t  *oscillator
 * @return fz_result_r
 */
fz_result_t fz_oscillator_apply(
                                fz_list_t       *samples,
                                fz_oscillator_t *oscillator);

#endif // _FZ_OSCILLATOR_H_