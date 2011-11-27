#ifndef _FZ_TYPES_H_
#define _FZ_TYPES_H_

typedef float fz_float_t;

typedef unsigned int fz_uint_t;

typedef fz_uint_t fz_result_t;

typedef fz_float_t fz_sample_value_t;

typedef fz_float_t fz_sample_instant_t;

typedef struct {
  fz_sample_instant_t *instants;
  fz_sample_value_t   *values;
  fz_uint_t           size;
} fz_sample_buffer_t;

typedef struct {
  fz_float_t x;
  fz_float_t y;
} fz_pointf_t;

typedef struct {
  fz_uint_t           state;
  fz_sample_value_t   *prototype;
  fz_uint_t           proto_size;
  fz_uint_t           proto_avail_size;
  pthread_mutex_t     lock;
} fz_form_t;

typedef struct {
  fz_form_t       form;
  fz_float_t      instant;
  fz_float_t      amplitude;
  fz_float_t      frequency;
  fz_float_t      sample_rate;
  pthread_mutex_t lock;
} fz_oscillator_t;

typedef struct {
  fz_pointf_t start;
  fz_pointf_t end;
  fz_pointf_t a;
  fz_pointf_t b;
  fz_float_t  tolerance;
} fz_bezier_t;

static const fz_result_t FZ_RESULT_SUCCESS = 0;

#endif // _FZ_TYPES_H_
