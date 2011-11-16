#ifndef _FZ_TYPES_H_
#define _FZ_TYPES_H_

typedef unsigned int fz_result_t;

typedef float fz_sample_value_t;

typedef float fz_sample_instant_t;

typedef struct {
    float x;
    float y;
} fz_pointf_t;

typedef struct {
    unsigned int        state;
    fz_sample_value_t   *cache;
    unsigned int        cache_size;
    unsigned int        requested_cache_size;
    pthread_mutex_t     lock;
} fz_form_t;

typedef struct {
    fz_form_t   *form;
    fz_pointf_t start;
    fz_pointf_t end;
    fz_pointf_t a;
    fz_pointf_t b;
    float       tolerance;
} fz_bezier_form_t;

static const fz_result_t FZ_RESULT_SUCCESS = 0;

#endif // _FZ_TYPES_H_