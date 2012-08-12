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

#ifndef _FZ_OSCILLATOR_H_
#define _FZ_OSCILLATOR_H_

#include "../types.h"

typedef struct {
    const fz_ptr_t  _class;
    fz_form_t      *form;
    fz_amp_t        amp;
    fz_real_t       phase;
} fz_oscdesc_t;

typedef struct {
    const fz_ptr_t  _class;
    fz_oscdesc_t   *descriptor;
    fz_frame_t      frame;
    fz_list_t      *framebuf;
    fz_real_t       freq;
    fz_uint_t       sample_rate;
} fz_oscillator_t;

/**
 *
 * @param  fz_oscillator_t *oscillator
 * @param  fz_list_t       *samples
 * @return fz_result_r
 */
fz_result_t fz_oscillator_apply(
                                fz_oscillator_t *oscillator,
                                fz_list_t       *samples);

const fz_ptr_t fz_oscdesc;
const fz_ptr_t fz_oscillator;

#endif // _FZ_OSCILLATOR_H_
