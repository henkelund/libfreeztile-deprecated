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

#ifndef _FZ_TYPES_H_
#define _FZ_TYPES_H_

#ifndef FZ_HASH_SIZE
#define FZ_HASH_SIZE 32
#endif

#include <pthread.h>
/*#include <gsl/gsl_fft_real_float.h>
#include <gsl/gsl_fft_halfcomplex_float.h>*/

typedef float             fz_float_t;

typedef float             fz_real_t;

typedef int               fz_int_t;

typedef unsigned int      fz_uint_t;

typedef unsigned long int fz_ulong_t;

typedef char              fz_char_t;

typedef int               fz_result_t;

typedef void*             fz_pointer_t;

typedef fz_ulong_t        fz_flags_t;

typedef fz_real_t         fz_amp_t;

typedef fz_real_t         fz_frame_t;

typedef pthread_mutex_t   fz_lock_t;

// fz_list_t function types
typedef fz_result_t       (*fz_lirm_f)(void *item);
typedef fz_int_t          (*fz_licmp_f)(void *a, void *b);

typedef struct {
    fz_float_t x;
    fz_float_t y;
} fz_pointf_t;

typedef struct {
    fz_pointer_t items;
    fz_uint_t    type_size;
    fz_char_t   *type_name;
    fz_uint_t    size;
    fz_uint_t    avail_size;
    fz_lirm_f    remove;
    fz_licmp_f   compare;
} fz_list_t;

typedef struct {
    fz_uint_t  state;
    fz_list_t *template;
    fz_char_t  version[FZ_HASH_SIZE];
} fz_form_t;

typedef struct {
    fz_form_t  *form;
    fz_frame_t  frame;
    fz_amp_t    amplitude;
    fz_float_t  frequency;
    fz_real_t   phase;
    fz_float_t  sample_rate;
    fz_list_t  *frame_buffer;
} fz_oscillator_t;

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

typedef struct {
    fz_form_t *adsr[4];
    fz_uint_t  sizes[4];
    fz_uint_t  position;
    fz_uint_t  state;
    fz_real_t  blend;
} fz_envelope_t;

// filter types
typedef struct fz_flt_tag fz_filter_t;
typedef fz_uint_t         (*fz_filter_f)(
                                         fz_filter_t *filter,
                                         fz_list_t   *samples,
                                         fz_list_t   *envelope);

struct fz_flt_tag {
    fz_filter_f    function;
    fz_flags_t     options;
    fz_envelope_t  envelope;
    fz_list_t     *env_buffer/*<fz_real_t>*/;
    fz_pointer_t   data;
};

typedef struct {
    fz_float_t rc;
    fz_amp_t   last;
} fz_filter_lowpass_t;

/*typedef struct {
    gsl_fft_real_wavetable_float        *real;
    gsl_fft_halfcomplex_wavetable_float *hc;
    gsl_fft_real_workspace_float        *work;
    fz_uint_t                           *size;
} fz_filter_equalizer_t;*/

#endif // _FZ_TYPES_H_
