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

#ifndef _FZ_TYPES_H_
#define _FZ_TYPES_H_

#include <stdlib.h>
#include <stdarg.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef float             fz_float_t;

typedef float             fz_real_t;

typedef int               fz_int_t;

typedef unsigned int      fz_uint_t;

typedef unsigned long int fz_ulong_t;

typedef char              fz_char_t;

typedef int               fz_result_t;

typedef void*             fz_ptr_t;

typedef size_t            fz_size_t;

typedef fz_ulong_t        fz_flags_t;

typedef fz_real_t         fz_amp_t;

typedef fz_real_t         fz_frame_t;

typedef char              fz_bool_t;

// fz base class
typedef struct {
    fz_size_t   size;
    fz_ptr_t  (*construct) (fz_ptr_t       self, va_list *args);
    fz_ptr_t  (*destruct)  (fz_ptr_t       self);
    fz_ptr_t  (*clone)     (const fz_ptr_t self);
    fz_int_t  (*compare)   (const fz_ptr_t self, const fz_ptr_t other);
} fz_object_t;

typedef struct {
    fz_float_t x;
    fz_float_t y;
} fz_pointf_t;

typedef struct {
    const fz_ptr_t   _scp; // static class pointer
    fz_ptr_t         items;
    fz_uint_t        type_size;
    fz_char_t       *type_name;
    fz_uint_t        size;
    fz_uint_t        avail_size;
    fz_flags_t       flags;
    fz_result_t    (*remove) (fz_ptr_t item);
    fz_int_t       (*compare)(const fz_ptr_t a, const fz_ptr_t b);
} fz_list_t;

typedef struct {
    const fz_ptr_t  _scp;
    fz_uint_t       state;
    fz_list_t      *template;
    fz_uint_t       version;
} fz_form_t;

typedef struct {
    const fz_ptr_t  _scp;
    fz_form_t      *form;
    fz_amp_t        amp;
    fz_real_t       phase;
} fz_osc_t;

typedef struct {
    fz_osc_t   *osc;
    fz_frame_t  frame;
    fz_list_t  *framebuf;
    fz_real_t   freq;
    fz_amp_t    amp;
    fz_uint_t   sample_rate;
} fz_octx_t;

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
    const fz_ptr_t   _scp;
    fz_uint_t      (*filter)(fz_ptr_t filter, fz_list_t *samples);
    fz_flags_t       options;
    fz_list_t       *env_buffer;
} fz_filter_t;

typedef struct {
    fz_filter_t _parent;
    fz_float_t  rc;
    fz_amp_t    last;
} fz_lowpass_t;

typedef fz_list_t/*<fz_osc_t*>*/ fz_voice_t;

typedef struct {
    const fz_ptr_t  _scp;
    fz_voice_t     *voice;
    fz_list_t      *octxs;
    fz_real_t       freq;
    fz_bool_t       is_active;
} fz_note_t;

/*typedef struct {
    fz_form_t *adsr[4];
    fz_uint_t  sizes[4];
    fz_uint_t  position;
    fz_uint_t  state;
    fz_real_t  mix;
} fz_envelope_t;*/

/*typedef struct {
    const fz_ptr_t  _scp;
    fz_list_t      *values;
} fz_regulator_t;*/


#endif // _FZ_TYPES_H_
