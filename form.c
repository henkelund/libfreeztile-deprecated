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

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "form.h"
#include "freeztile.h"
#include "util/list.h"

// ### PRIVATE ###

static
fz_ptr_t
_fz_form_construct(
                  const fz_ptr_t  self,
                  va_list        *args)
{
    fz_form_t *_self = (fz_form_t*) self;
    (void) args;
    _self->template  = fz_list_new(fz_amp_t);
    _self->state     = FZ_FORM_STATE_NONE;
    _self->version   = 0;
    return _self;
}

static
fz_ptr_t
_fz_form_destruct(fz_ptr_t self)
{
    fz_form_t *_self = (fz_form_t*) self;
    fz_release(_self->template);
    return _self;
}

static const fz_object_t _fz_form = {
    sizeof (fz_form_t),
    _fz_form_construct,
    _fz_form_destruct,
    NULL,
    NULL
};

// ### PUBLIC ###

const fz_ptr_t fz_form = (const fz_ptr_t) &_fz_form;

fz_result_t
fz_form_apply(
              fz_form_t   *form,
              fz_list_t   *frames,
              fz_list_t   *amplitudes)
{
    fz_uint_t i;
    fz_uint_t frame;

    if (!fz_list_type(frames, fz_frame_t) || !fz_list_type(amplitudes, fz_amp_t)) {
        return FZ_RESULT_INVALID_ARG;
    }

    if (fz_list_size(form->template) > 0) {
        for (i = 0; i < fz_list_size(frames) && i < fz_list_size(amplitudes); ++i) {
            frame = (fz_uint_t)
                        (fz_list_val(frames, i, fz_frame_t) *
                                        fz_list_size(form->template));
            fz_list_val(amplitudes, i, fz_amp_t) +=
                    fz_list_val(form->template, frame, fz_amp_t);
        }
    }
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_curve_render(
                fz_curve_t  *curve,
                fz_form_t   *form)
{
    // declare vars
    fz_uint_t  i, hash;     // counter, version
    fz_float_t pos,
               t,
               t2,          // pow(t, 2)
               ti,          // inverted t
               ti2,         // pow(ti, 2)
               f,
               d,           // f deriv
               c0y,
               c1y,
               c2y,
               c3y;

    // don't bother to render if curve hasn't changed (force option?)
    hash = fz_hash((fz_char_t*) curve, sizeof (fz_curve_t));
    if (hash == form->version) {
        return FZ_RESULT_SUCCESS;
    }

    // assign vars
    c0y = curve->start,
    c1y = (3*curve->a.y)-(3*curve->start),
    c2y = (3*curve->start)-(2*(3*curve->a.y))+(3*curve->b.y),
    c3y = curve->end-curve->start+(3*curve->a.y)-(3*curve->b.y);

    // render it
    for (i = 0; i < fz_list_size(form->template); ++i) {
        t = pos = ((float) i)/fz_list_size(form->template); // first approx of t = x
        d = 1;
        f = (t > 0.f ? 1 : 0);
        while (fabs(f/d) > FZ_NR_TOL) {
            ti = 1 - t;
            t2 = t*t;
            ti2 = ti*ti;

            f = (3*ti2*t*curve->a.x) +
                (3*ti*t2*curve->b.x) +
                (t2*t) - pos;

            d = (3*curve->a.x*(1 - 4*t + 3*t2)) +
                (3*curve->b.x*(2*t - 3*t2)) +
                (3*t2);

            t -= f/d;
        }
        fz_list_val(form->template, i, fz_amp_t) = c0y+t*(c1y+t*(c2y+t*c3y));
    }

    // update form version with curve hash
    form->version = hash;
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_multicurve_normalize_shares(
                               fz_list_t  *multicurve)
{
    fz_float_t total_share;
    fz_uint_t  i;

    if (multicurve == NULL || !fz_list_type(multicurve, fz_mccurve_t)) {
        return FZ_RESULT_INVALID_ARG;
    }

    total_share = 0.f;
    for (i = 0; i < fz_list_size(multicurve); ++i) {
        total_share += fz_list_ref(multicurve, i, fz_mccurve_t)->share;
    }
    for (i = 0; i < fz_list_size(multicurve); ++i) {
        fz_list_ref(multicurve, i, fz_mccurve_t)->share /= total_share;
    }

    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_multicurve_render(
                     fz_list_t  *multicurve,
                     fz_form_t  *form)
{
    fz_result_t  result;
    fz_uint_t    i, j, k, offset, size;
    fz_mccurve_t *mccurve;
    if ((result = fz_multicurve_normalize_shares(multicurve)) != FZ_RESULT_SUCCESS) {
        return result;
    }
    offset = 0;
    for (i = 0; i < fz_list_size(multicurve); ++i) {
        mccurve = fz_list_ref(multicurve, i, fz_mccurve_t);
        // create curve cache form if NULL
        if (mccurve->form == NULL) {
            mccurve->form = fz_new(fz_form);
        }
        // make room for form
        if ((result = fz_list_clear(
                mccurve->form->template,
                fz_list_size(form->template))) != FZ_RESULT_SUCCESS) {
            return result;
        }
        // render
        if ((result = fz_curve_render(&mccurve->curve, mccurve->form))
                != FZ_RESULT_SUCCESS) {
            return result;
        }

        size = fz_list_size(form->template)*mccurve->share;
        for (j = 0; j < size; ++j) {
            // maybe find average instead?
            k = (fz_uint_t)((((fz_float_t)j)/size)*
                    fz_list_size(mccurve->form->template));
            fz_list_val(form->template, offset + j, fz_amp_t) =
                    fz_list_val(mccurve->form->template, k, fz_amp_t);
        }
        offset += mccurve->share*fz_list_size(form->template);
    }
    return FZ_RESULT_SUCCESS;
}
