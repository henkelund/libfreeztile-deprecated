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

#ifndef _FZ_FREEZTILE_H_
#define _FZ_FREEZTILE_H_

#undef BEGIN_C_DECLS
#undef END_C_DECLS
#ifdef __cplusplus
# define BEGIN_C_DECLS extern "C" {
# define END_C_DECLS }
#else
# define BEGIN_C_DECLS /* empty */
# define END_C_DECLS /* empty */
#endif

#undef PARAMS
#if defined (__STDC__) || defined (_AIX) \
        || (defined (__mips) && defined (_SYSTYPE_SVR4)) \
        || defined(WIN32) || defined(__cplusplus)
# define PARAMS(protos) protos
#else
# define PARAMS(protos) ()
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>

BEGIN_C_DECLS

//#define FZ_DEBUG 1

#if defined FZ_DEBUG && FZ_DEBUG == 1
    #include <stdio.h>
    #define FZ_IF_DEBUG(code) code
    #define fzdebug(...) fprintf(stderr, __VA_ARGS__)
#else
    #define FZ_IF_DEBUG(code) /* code */
    #define fzdebug(...) /* __VA_ARGS__ */
#endif

/**
 * FreeZtile return values
 *
 */
#define FZ_RESULT_SUCCESS         0
#define FZ_RESULT_NOT_IMPLEMENTED 1
#define FZ_RESULT_MALLOC_ERROR    2
#define FZ_RESULT_IOOB_ERROR      3
#define FZ_RESULT_INVALID_ARG     4

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

typedef struct {
    fz_size_t   size;
    fz_ptr_t  (*construct) (fz_ptr_t       self, va_list *args);
    fz_ptr_t  (*destruct)  (fz_ptr_t       self);
    fz_ptr_t  (*clone)     (const fz_ptr_t self);
    fz_int_t  (*compare)   (const fz_ptr_t self, const fz_ptr_t other);
} fz_object_t;

/**
 *
 * @param  fz_size_t size
 * @return fz_ptr_t
 */
fz_ptr_t    fz_malloc PARAMS((fz_size_t size));

/**
 *
 * @param  fz_ptr_t ptr
 */
void        fz_free PARAMS((fz_ptr_t ptr));

/**
 *
 * @param  fz_ptr_t type
 * @param  ...
 * @return fz_ptr_t New object of given type
 */
fz_ptr_t    fz_new PARAMS((const fz_ptr_t type, ...));

/**
 * Increase reference count of obj
 *
 * @param  fz_ptr_t obj
 * @return fz_ptr_t obj
 */
fz_ptr_t    fz_retain PARAMS((fz_ptr_t obj));

/**
 *
 * @param  fz_ptr_t self
 */
void        fz_release PARAMS((fz_ptr_t self));

/**
 *
 * @param  const fz_ptr_t object
 * @return fz_ptr_t
 */
fz_ptr_t    fz_clone PARAMS((const fz_ptr_t));

/**
 *
 * @param  const fz_ptr_t a
 * @param  const fz_ptr_t b
 * @return fz_int_t
 */
fz_int_t    fz_compare PARAMS((const fz_ptr_t, const fz_ptr_t));

/**
 * Returns a DJBX33X hash integer of the given data
 *
 * @param  data
 * @param  size
 * @return
 */
uint32_t    fz_hash PARAMS((
                            const fz_char_t *data,
                            fz_size_t        size));

/**
 *
 * @param  fz_result_t result
 * @return const char*
 */
const char* fz_result_string PARAMS((fz_result_t result));

END_C_DECLS

#endif // _FZ_FREEZTILE_H_
