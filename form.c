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

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "form.h"
#include "freeztile.h"
#include "list.h"

fz_result_t 
fz_form_create(fz_form_t **form)
{
    fz_result_t result;
    fz_form_t *f = malloc(sizeof(fz_form_t));
    if (f == NULL) {
        return FZ_RESULT_MALLOC_ERROR;
    }
    if ((result = fz_list_create(&f->template, sizeof(fz_splval_t))) 
            != FZ_RESULT_SUCCESS) {
        free(f);
        return result;
    }
    f->state = FZ_FORM_STATE_NONE;
    f->version = 0;
    *form = f;
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_form_destroy(fz_form_t **form)
{
    fz_result_t result;
    fz_form_t   *f = *form;
    if ((result = fz_list_destroy(&f->template)) != FZ_RESULT_SUCCESS) {
        return result;
    }
    free(f);
    *form = NULL;
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_form_apply(
              fz_list_t   *samples,
              fz_form_t   *form)
{
    fz_uint_t i;
    fz_uint_t instant;
    
    if (!FZ_LIST_TYPE(samples, fz_sample_t)) {
        return FZ_RESULT_INVALID_ARG;
    }
    
    if (form->template->size > 0) {
        for (i = 0; i < samples->size; ++i) {
            instant = (fz_uint_t)
                    (FZ_LIST_REF(samples, i, fz_sample_t)->instant * 
                        form->template->size);
            FZ_LIST_REF(samples, i, fz_sample_t)->value = 
                    FZ_LIST_VAL(form->template, instant, fz_splval_t);
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
    fz_uint_t  i;           // counter
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
    if (curve->version == form->version) {
        return FZ_RESULT_SUCCESS;
    }
    
    // assign vars
    c0y = curve->start,
    c1y = (3*curve->a.y)-(3*curve->start),
    c2y = (3*curve->start)-(2*(3*curve->a.y))+(3*curve->b.y),
    c3y = curve->end-curve->start+(3*curve->a.y)-(3*curve->b.y);

    // render it
    for (i = 0; i < form->template->size; ++i) {
        t = pos = ((float) i)/form->template->size; // first approximation of t = x
        d = 1;
        f = (t > 0.f ? 1 : 0);
        while (fabs(f/d) > curve->tolerance) {
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
        FZ_LIST_VAL(form->template, i, fz_splval_t) = c0y+t*(c1y+t*(c2y+t*c3y));
    }
    
    form->version = curve->version;
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_multicurve_normalize_shares(
                               fz_list_t  *multicurve)
{
    fz_float_t total_share;
    fz_uint_t  i;
    
    if (multicurve == NULL || !FZ_LIST_TYPE(multicurve, fz_mccurve_t)) {
        return FZ_RESULT_INVALID_ARG;
    }

    total_share = 0.f;
    for (i = 0; i < multicurve->size; ++i) {
        total_share += FZ_LIST_REF(multicurve, i, fz_mccurve_t)->share;
    }
    for (i = 0; i < multicurve->size; ++i) {
        FZ_LIST_REF(multicurve, i, fz_mccurve_t)->share /= total_share;
    }
    
    return FZ_RESULT_SUCCESS;
}

#include <stdio.h>

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
    for (i = 0; i < multicurve->size; ++i) {
        mccurve = FZ_LIST_REF(multicurve, i, fz_mccurve_t);
        // create curve cache form if NULL
        if (mccurve->form == NULL && (result = fz_form_create(&mccurve->form)) 
                != FZ_RESULT_SUCCESS) {
            return result;
        }
        // make room for form @todo don't hard code size
        if ((result = fz_list_clear(
                mccurve->form->template,
                form->template->size)) != FZ_RESULT_SUCCESS) {
            return result;
        }
        // render
        if ((result = fz_curve_render(&mccurve->curve, mccurve->form))
                != FZ_RESULT_SUCCESS) {
            return result;
        }
        
        size = form->template->size*mccurve->share;
        for (j = 0; j < size; ++j) {
            // maybe find average instead?
            k = (fz_uint_t)((((fz_float_t)j)/size)*mccurve->form->template->size);
            FZ_LIST_VAL(form->template, offset + j, fz_splval_t) = 
                    FZ_LIST_VAL(mccurve->form->template, k, fz_splval_t);
        }
        offset += mccurve->share*form->template->size;
    }
    return FZ_RESULT_SUCCESS;
}