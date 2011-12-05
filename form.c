#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "form.h"

const char*
fz_result_string(fz_result_t result)
{
  if (result == FZ_RESULT_SUCCESS) {
    return "success";
  } else if (result == FZ_RESULT_NOT_IMPLEMENTED) {
    return "not implemented";
  }
  return "unknown result";
}

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

fz_result_t
fz_form_apply(
              fz_sample_buffer_t *samples,
              fz_form_t          *form)
{
  fz_uint_t i;
  fz_uint_t instant;
  for (i = 0; i < samples->size; ++i) {
    instant = (fz_uint_t)(((fz_float_t)form->proto_size)*samples->instants[i]);
    samples->values[i] = form->prototype[instant];
  }
  return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_oscillator_malloc(fz_oscillator_t *oscillator)
{
  oscillator = NULL;
  return FZ_RESULT_NOT_IMPLEMENTED;
}

fz_result_t
fz_oscillator_free(fz_oscillator_t *oscillator)
{
  oscillator = NULL;
  return FZ_RESULT_NOT_IMPLEMENTED;
}

fz_result_t     
fz_oscillator_apply(
                    fz_sample_buffer_t *samples,
                    fz_oscillator_t *oscillator)
{
  fz_uint_t i;
  // calculate the progress rate
  fz_float_t step_size =  
    1.f/(oscillator->sample_rate/oscillator->frequency);
    // sample rate divided by frequeny gives number of
    // samples per period
 
  // fill the instants part of the sample buffer
  for (i = 0; i < samples->size; ++i) {
    samples->instants[i] = oscillator->instant;
    oscillator->instant += step_size;
    if (oscillator->instant >= 1.0f) {
      oscillator->instant -= 1.0f;
    }
  }

  fz_form_apply(samples, &(oscillator->form));
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
fz_bezier_build_prototype(
                          fz_bezier_t *bezier,
                          fz_form_t   *form,
                          fz_uint_t   size)
{
    // declare vars
    fz_uint_t  p_size,      // prototype size in samples
               p_byte_size, // .. in bytes
               i;           // counter
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
    p_size = size; //@todo bounds check
    p_byte_size = sizeof(fz_sample_value_t)*p_size;
    c0y = bezier->start.y,
    c1y = (3*bezier->a.y)-(3*bezier->start.y),
    c2y = (3*bezier->start.y)-(2*(3*bezier->a.y))+(3*bezier->b.y),
    c3y = bezier->end.y-bezier->start.y+(3*bezier->a.y)-(3*bezier->b.y);
    
    // lock form
    _fz_form_acquire(form);
        
    // free old space if its too small
    if (p_size > form->proto_avail_size && form->prototype != NULL) {
        free(form->prototype);
        form->prototype = NULL;
    }
    // allocate space prototype is empty
    if (form->prototype == NULL) {
        form->prototype = (fz_sample_value_t*) malloc(p_byte_size);
        form->proto_avail_size = p_size;
    }
    form->proto_size = p_size;
    
    // render it
    for (i = 0; i < p_size; ++i) {
        t = pos = ((float) i)/p_size; // first approximation of t = x
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
    
    // unlock form
    _fz_form_release(form);
    return FZ_RESULT_SUCCESS;
}
