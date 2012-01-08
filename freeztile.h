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

#ifndef _FZ_FREEZTILE_H_
#define _FZ_FREEZTILE_H_

#include <stdarg.h>
#include "types.h"

/**
 * FreeZtile return values
 * 
 */
#define FZ_RESULT_SUCCESS          0
#define FZ_RESULT_NOT_IMPLEMENTED -1
#define FZ_RESULT_MALLOC_ERROR    -2
#define FZ_RESULT_LOCK_ERROR      -3
#define FZ_RESULT_IOOB_ERROR      -4
#define FZ_RESULT_INVALID_ARG     -5

/**
 * 
 * @param  fz_ptr_t type
 * @param  ...
 * @return fz_ptr_t New object of given type
 */
fz_ptr_t fz_new(const fz_ptr_t type, ...);

/**
 * Allocate & initialize a mutex lock
 * 
 * @param  fz_lock_t **lock
 * @return fz_result
 */
fz_result_t fz_lock_create(fz_lock_t **lock);

/**
 * Destroy a mutex lock
 * 
 * @param  fz_lock_t **lock
 * @return fz_result
 */
fz_result_t fz_lock_destroy(fz_lock_t **lock);

/**
 * Acquire a mutex lock
 * 
 * @param  fz_lock_t *lock
 * @return fz_result
 */
fz_result_t fz_lock_acquire(fz_lock_t *lock);

/**
 * Release a mutex lock
 * 
 * @param  fz_lock_t *lock
 * @return fz_result
 */
fz_result_t fz_lock_release(fz_lock_t *lock);

/**
 * This function is implemented using the RFC 1321 MD5 Message-Digest Algorithm
 * 
 * @see http://www.ietf.org/rfc/rfc1321.txt
 * @param char      *buffer
 * @param void      *data
 * @param fz_uint_t size
 * @return 
 */
fz_result_t fz_hash(char *buffer/*[FZ_HASH_SIZE]*/, void *data, fz_uint_t size);

/**
 * 
 * @param  fz_result_t result
 * @return const char*
 */
const char* fz_result_string(fz_result_t result);

#endif // _FZ_FREEZTILE_H_