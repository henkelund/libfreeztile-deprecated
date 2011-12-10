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

fz_result_t 
fz_form_create(fz_form_t **form)
{
    fz_form_t *f = malloc(sizeof(fz_form_t));
    if (f == NULL) {
        return FZ_RESULT_MALLOC_ERROR;
    }
    f->state = FZ_FORM_STATE_NONE;
    f->prototype = NULL;
    f->proto_size = 0;
    f->proto_avail_size = 0;
    *form = f;
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_form_destroy(fz_form_t **form)
{
    return FZ_RESULT_NOT_IMPLEMENTED;
}

fz_result_t
fz_form_apply(
              fz_splbuf_t *samples,
              fz_form_t   *form)
{
    fz_uint_t i;
    fz_uint_t instant;
    if (form->proto_size > 0) {
        for (i = 0; i < samples->size; ++i) {
            instant = (fz_uint_t)(((fz_float_t)form->proto_size)*samples->instants[i]);
            samples->values[i] = form->prototype[instant];
        }
    }
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_form_resize(
               fz_form_t *form,
               fz_uint_t size)
{
    // @todo Thread safety
    fz_uint_t   i,           // new size counter
                pi,          // prototype size counter
                malloc_size; // 
    
    fz_splval_t *prototype; // new prototype
    
    if (size < form->proto_size) {
        
        for (i = 0; i < size; ++i) {
            pi = (fz_uint_t)((((fz_float_t)i)/size)*form->proto_size);
            form->prototype[i] = form->prototype[pi];
        }
        form->proto_size = size;
        
    } else if (size > form->proto_size) {
        
        if (size > form->proto_avail_size) {
            
            malloc_size = (fz_uint_t) pow(2, ceil(log(size)/log(2)));
            prototype = malloc(malloc_size*sizeof(fz_splval_t));
            memset(prototype, 0, malloc_size*sizeof(fz_splins_t));
            if (form->prototype != NULL) {
                for (i = 0; i < size; ++i) {
                    pi = (fz_uint_t)((((fz_float_t)i)/size)*form->proto_size);
                    prototype[i] = form->prototype[pi];
                }
                free(form->prototype);
            }
            form->prototype = prototype;
            form->proto_avail_size = malloc_size;
        } else {
            
            for (i = size - 1; size <= 0; --i) {
                pi = (fz_uint_t)((((fz_float_t)i)/size)*form->proto_size);
                form->prototype[i] = form->prototype[pi];
            }
        }
        form->proto_size = size;
    } /*else if (size == form->proto_size) {
        // nothing to do..
    }*/
    return FZ_RESULT_SUCCESS;
}


fz_result_t 
fz_bezier_build_prototype(
                          fz_bezier_t *bezier,
                          fz_form_t   *form)
{
    // @todo Thread safety
    
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
    
    // assign vars
    c0y = bezier->start.y,
    c1y = (3*bezier->a.y)-(3*bezier->start.y),
    c2y = (3*bezier->start.y)-(2*(3*bezier->a.y))+(3*bezier->b.y),
    c3y = bezier->end.y-bezier->start.y+(3*bezier->a.y)-(3*bezier->b.y);

    // render it
    for (i = 0; i < form->proto_size; ++i) {
        t = pos = ((float) i)/form->proto_size; // first approximation of t = x
        d = 1;
        f = (t > 0.f ? 1 : 0);
        while (fabs(f/d) > bezier->tolerance) {
            ti = 1 - t;
            t2 = t*t;
            ti2 = ti*ti;
            
            f = (ti2*ti*bezier->start.x) +
                (3*ti2*t*bezier->a.x) +
                (3*ti*t2*bezier->b.x) +
                (t2*t*bezier->end.x) -
                pos;
            
            d = -
                (3*ti2*bezier->start.x) +
                (3*bezier->a.x*(1 - 4*t + 3*t2)) +
                (3*bezier->b.x*(2*t - 3*t2)) +
                (3*t2*bezier->end.x);

            t -= f/d;
        }
        form->prototype[i] = c0y+t*(c1y+t*(c2y+t*c3y));
    }
    
    return FZ_RESULT_SUCCESS;
}
