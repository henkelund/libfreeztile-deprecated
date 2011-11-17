#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "form.h"

fz_result_t 
fz_form_init(fz_form_t *form)
{
    form->state = FZ_FORM_STATE_NONE;
    form->prototype = NULL;
    form->proto_size = 0;
    form->proto_avail_size = 0;
    pthread_mutex_init(&(form->lock), NULL);
    return FZ_RESULT_SUCCESS;
}

void 
_fz_form_acquire(fz_form_t *form)
{
    pthread_mutex_lock(&(form->lock));
}

void 
_fz_form_release(fz_form_t *form)
{
    pthread_mutex_unlock(&(form->lock));
}

fz_result_t 
fz_bezier_build_prototype(fz_bezier_form_t *bf)
{
    // declare vars
    unsigned int    p_size,      // prototype size in samples
                    p_byte_size, // .. in bytes
                    i;           // counter
    float   pos,
            t,
            t2,  // pow(t, 2)
            ti,  // inverted t
            ti2, // pow(ti, 2)
            f,
            d,   // f deriv
            c0y,
            c1y,
            c2y,
            c3y;
    
    // lock form
    _fz_form_acquire(bf->form);
    
    // assign vars
    p_size = 10;
    p_byte_size = sizeof(fz_sample_value_t)*p_size;
    c0y = bf->start.y,
    c1y = (3*bf->a.y)-(3*bf->start.y),
    c2y = (3*bf->start.y)-(2*(3*bf->a.y))+(3*bf->b.y),
    c3y = bf->end.y-bf->start.y+(3*bf->a.y)-(3*bf->b.y);
    
    // free old space if its too small
    if (p_size > bf->form->proto_avail_size && bf->form->prototype != NULL) {
        free(bf->form->prototype);
        bf->form->prototype = NULL;
    }
    // allocate space prototype is empty
    if (bf->form->prototype == NULL) {
        bf->form->prototype = (fz_sample_value_t*) malloc(p_byte_size);
        bf->form->proto_avail_size = p_size;
    }
    bf->form->proto_size = p_size;
    
    // render it
    for (i = 0; i < p_size; ++i) {
        t = pos = ((float) i)/p_size; // first approximation of t = x
        d = 1;
        f = (t > 0.f ? 1 : 0);
        while (fabs(f/d) > bf->tolerance) {
            ti = 1 - t;
            t2 = t*t;
            ti2 = ti*ti;
            
            f = (ti2*ti*bf->start.x) +
                (3*ti2*t*bf->a.x) +
                (3*ti*t2*bf->b.x) +
                (t2*t*bf->end.x) -
                pos;
            
            d = -
                (3*ti2*bf->start.x) +
                (3*bf->a.x*(1 - 4*t + 3*t2)) +
                (3*bf->b.x*(2*t - 3*t2)) +
                (3*t2*bf->end.x);

            t -= f/d;
        }
        bf->form->prototype[i] = c0y+t*(c1y+t*(c2y+t*c3y));
    }
    
    // unlock form
    _fz_form_release(bf->form);
    return FZ_RESULT_SUCCESS;
}