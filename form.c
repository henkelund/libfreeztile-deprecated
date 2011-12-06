#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "form.h"

const char*
fz_result_string(fz_result_t result)
{
  if (result == FZ_RESULT_SUCCESS) {
    return "success";
  } else if (result == FZ_RESULT_NOT_IMPLEMENTED) {
    return "not implemented";
  } else if (result == FZ_RESULT_MALLOC_ERROR) {
    return "memory allocation error";
  } else if (result == FZ_RESULT_MUTEX_ERROR) {
    return "mutex error";
  }
  return "unknown result";
}

fz_result_t 
fz_lock_create(fz_lock_t **lock)
{
  fz_lock_t *mutex = malloc(sizeof(fz_lock_t));
  if (mutex == NULL) {
    return FZ_RESULT_MALLOC_ERROR;
  }
  if (pthread_mutex_init(mutex, NULL) != 0) {
    return FZ_RESULT_MUTEX_ERROR;
  }
  *lock = mutex;
  return FZ_RESULT_SUCCESS;
}

fz_result_t 
fz_lock_destroy(fz_lock_t **lock)
{
  return FZ_RESULT_NOT_IMPLEMENTED;
}

fz_result_t
fz_lock_acquire(fz_lock_t *lock)
{
  fz_result_t result;
  if ((result = pthread_mutex_lock(lock)) != 0) {
     return FZ_RESULT_MUTEX_ERROR;
  }
  return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_lock_release(fz_lock_t *lock)
{
  fz_result_t result;
  if ((result = pthread_mutex_unlock(lock)) != 0) {
     return FZ_RESULT_MUTEX_ERROR;
  }
  return FZ_RESULT_SUCCESS;
}

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
  for (i = 0; i < samples->size; ++i) {
    instant = (fz_uint_t)(((fz_float_t)form->proto_size)*samples->instants[i]);
    samples->values[i] = form->prototype[instant];
  }
  return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_oscillator_create(fz_oscillator_t **oscillator)
{
  fz_result_t result;
  fz_oscillator_t *osc = malloc(sizeof(fz_oscillator_t));
  if (osc == NULL) {
    return FZ_RESULT_MALLOC_ERROR;
  }
  if ((result = fz_form_create(&osc->form)) != 0) {
    return result;
  }
  osc->instant = 0.0f;
  osc->amplitude = 1.0f;
  osc->frequency = 440.0f;
  osc->sample_rate = 44100;
  *oscillator = osc;
  return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_oscillator_destroy(fz_oscillator_t **oscillator)
{
  return FZ_RESULT_NOT_IMPLEMENTED;
}

fz_result_t     
fz_oscillator_apply(
                    fz_splbuf_t     *samples,
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

  fz_form_apply(samples, oscillator->form);
  return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_splbuf_create(
                 fz_splbuf_t **buffer,
                 fz_uint_t   size)
{
  fz_result_t result;
  fz_splbuf_t *buf = malloc(sizeof(fz_splbuf_t));
  if (buf == NULL) {
    return FZ_RESULT_MALLOC_ERROR;
  }
  buf->instants = NULL;
  buf->values = NULL;
  buf->size = 0;
  buf->avail_size = 0;
  if ((result = fz_lock_create(&buf->lock)) != 0) {
    free(buf);
    return result;
  }
  if (size > 0 && (result = fz_splbuf_resize(buf, size)) != 0) {
    free(buf);
    return result;
  }
  *buffer = buf;
  return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_splbuf_destroy(fz_splbuf_t **buffer)
{
  return FZ_RESULT_NOT_IMPLEMENTED;
}

fz_result_t
fz_splbuf_resize(
                 fz_splbuf_t *buffer,
                 fz_uint_t   size)
{
  // declare variables
  fz_uint_t malloc_size, i;
  fz_splins_t *new_instants;
  fz_splval_t *new_values;
  
  // request access to the buffer
  fz_lock_acquire(buffer->lock);
  
  if (size <= buffer->avail_size) {
    // requested size already available
    // just raise the public value
    buffer->size = size;

  } else { 
    // requested size not availble -> allocate
    // expand the requested size to the nearest (larger) power of two
    // this way we will have a margin and might avoid resizing to often
    malloc_size = (fz_uint_t) pow(2, ceil(log(size)/log(2)));
    new_instants = malloc(malloc_size*sizeof(fz_splins_t));
    memset(new_instants, 0, malloc_size*sizeof(fz_splins_t));
    new_values = malloc(malloc_size*sizeof(fz_splval_t));
    memset(new_values, 0, malloc_size*sizeof(fz_splval_t));
    
    // transfer old values 
    // this might fail if the given buffer struct is not
    // initialized properly.
    if (buffer->instants != NULL && buffer->values != NULL) {
      for (i = 0; i < buffer->size; ++i) {
        new_instants[i] = buffer->instants[i];
        new_values[i] = buffer->values[i];
      }
      free(buffer->instants);
      free(buffer->values);
    }
    
    // update buffer variables
    buffer->instants = new_instants;
    buffer->values = new_values;
    buffer->avail_size = malloc_size;
    buffer->size = size;
  }

  fz_lock_release(buffer->lock);
  return FZ_RESULT_SUCCESS;
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
    p_byte_size = sizeof(fz_splval_t)*p_size;
    c0y = bezier->start.y,
    c1y = (3*bezier->a.y)-(3*bezier->start.y),
    c2y = (3*bezier->start.y)-(2*(3*bezier->a.y))+(3*bezier->b.y),
    c3y = bezier->end.y-bezier->start.y+(3*bezier->a.y)-(3*bezier->b.y);
    
    // free old space if its too small
    if (p_size > form->proto_avail_size && form->prototype != NULL) {
        free(form->prototype);
        form->prototype = NULL;
    }
    // allocate space prototype is empty
    if (form->prototype == NULL) {
        form->prototype = malloc(p_byte_size);
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
    
    return FZ_RESULT_SUCCESS;
}
