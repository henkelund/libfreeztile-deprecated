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

#ifndef _FZ_FORM_H_
#define _FZ_FORM_H_

#include "types.h"

static const unsigned int FZ_FORM_STATE_NONE       = 0;
static const unsigned int FZ_FORM_STATE_APPLYING   = (1 << 0);

/**
 * Allocate & initialize the given form struct
 * 
 * @param  fz_form_t **form
 * @return fz_result
 */
fz_result_t fz_form_create(fz_form_t **form);

/**
 * Free resources of the given form struct
 * 
 * @param  fz_form_t **form
 * @return fz_result
 */
fz_result_t fz_form_destroy(fz_form_t **form);

/**
 *
 * @param  fz_splbuf_t *samples
 * @param  fz_form_t *form
 * @return fz_result
 */
fz_result_t fz_form_apply(
                          fz_splbuf_t *samples,
                          fz_form_t   *form);

/**
 *
 * @param  fz_form_t   *form
 * @param  fz_uint_t   size
 * @return fz_result_t
 */
fz_result_t fz_form_resize(
                           fz_form_t *form,
                           fz_uint_t size);

/**
 * 
 * @param  fz_curve_t  *curve
 * @param  fz_form_t   *form
 * @return fz_result_t 
 */
fz_result_t fz_curve_build_prototype(
                                     fz_curve_t  *curve,
                                     fz_form_t   *form);

/**
 * 
 * @param  fz_multicurve_t **multicurve
 * @return fz_result_t
 */
fz_result_t fz_multicurve_create(fz_multicurve_t **multicurve);

/**
 * 
 * @param fz_multicurve_t *multicurve
 * @param fz_curve_t      *curve
 * @param fz_int_t        *curve
 * @return 
 */
fz_result_t fz_multicurve_add(
                              fz_multicurve_t *multicurve,
                              fz_curve_t      *curve,
                              fz_int_t        pos);

/**
 * 
 * @param  fz_multicurve_t  *curve
 * @param  fz_form_t        *form
 * @return fz_result_t 
 */
fz_result_t fz_multicurve_build_prototype(
                                          fz_multicurve_t  *multicurve,
                                          fz_form_t        *form);

#endif // _FZ_FORM_H_
