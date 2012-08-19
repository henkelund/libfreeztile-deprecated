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

#ifndef _FZ_FORM_H_
#define _FZ_FORM_H_

#include "types.h"
#include "util/list.h"

#ifndef FZ_NR_TOL
#define FZ_NR_TOL 0.0001f
#endif

#define DEFAULT_CURVE { \
        .start     = 0.f, \
        .end       = 0.f, \
        .a         = {.x = 0.25f, .y = 3.f}, \
        .b         = {.x = 0.75f, .y = -3.f}, \
    }

#define FZ_FORM_STATE_NONE      0;
#define FZ_FORM_STATE_APPLYING (1 << 0);

typedef struct {
    const fz_ptr_t  _scp;
    fz_uint_t       state;
    fz_list_t      *template;
    fz_uint_t       version;
} fz_form_t;

typedef struct {
    fz_float_t x;
    fz_float_t y;
} fz_pointf_t;

typedef struct {
    fz_amp_t    start;
    fz_amp_t    end;
    fz_pointf_t a;
    fz_pointf_t b;
} fz_curve_t;

// multicurve curve
typedef struct {
    fz_form_t  *form;
    fz_curve_t  curve;
    fz_float_t  share;
} fz_mccurve_t;

/**
 *
 * @param  fz_form_t *form
 * @param  fz_list_t *frames
 * @param  fz_list_t *amplitudes
 * @return fz_result
 */
fz_result_t fz_form_apply(
                          fz_form_t   *form,
                          fz_list_t   *frames,
                          fz_list_t   *amplitudes);

/**
 *
 * @param  fz_curve_t  *curve
 * @param  fz_form_t   *form
 * @return fz_result_t
 */
fz_result_t fz_curve_render(
                            fz_curve_t  *curve,
                            fz_form_t   *form);

/**
 *
 * @param fz_list_t *multicurve
 * @return
 */
fz_result_t fz_multicurve_normalize_shares(
                                           fz_list_t  *multicurve);

/**
 *
 * @param  fz_list_t   *curve
 * @param  fz_form_t   *form
 * @return fz_result_t
 */
fz_result_t fz_multicurve_render(
                                 fz_list_t  *multicurve,
                                 fz_form_t  *form);

const fz_ptr_t fz_form;

#endif // _FZ_FORM_H_
