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

#include "types.h"

static const fz_result_t FZ_RESULT_SUCCESS         = 0;
static const fz_result_t FZ_RESULT_NOT_IMPLEMENTED = -(1 << 0);
static const fz_result_t FZ_RESULT_MALLOC_ERROR    = -(1 << 1);
static const fz_result_t FZ_RESULT_LOCK_ERROR      = -(1 << 2);
static const fz_result_t FZ_RESULT_IOOB_ERROR      = -(1 << 3);

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
 *
 * @param  fz_splbuf_t**
 * @param  fz_uint_t 
 * @return fz_result_t
 */
fz_result_t fz_splbuf_create(
                             fz_splbuf_t **buffer,
                             fz_uint_t   size);
/**
 *
 * @param  fz_splbuf_t**
 * @return fz_result_t
 */
fz_result_t fz_splbuf_destroy(fz_splbuf_t **buffer);

/**
 *
 * @param  fz_splbuf_t*
 * @param  fz_uint_t
 * @return fz_result_t
 */
fz_result_t fz_splbuf_resize(
                             fz_splbuf_t *buffer,
                             fz_uint_t   size);

/**
 * 
 * @param  fz_result_t result
 * @return const char*
 */
const char* fz_result_string(fz_result_t result);

#endif // _FZ_FREEZTILE_H_