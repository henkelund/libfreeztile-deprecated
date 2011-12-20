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

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "freeztile.h"
#include "lib/md5/global.h"
#include "lib/md5/md5.h"

fz_result_t 
fz_lock_create(fz_lock_t **lock)
{
    fz_lock_t *mutex = malloc(sizeof(fz_lock_t));
    if (mutex == NULL) {
        return FZ_RESULT_MALLOC_ERROR;
    }
    if (pthread_mutex_init(mutex, NULL) != 0) {
        free(mutex);
        return FZ_RESULT_LOCK_ERROR;
    }
    *lock = mutex;
    return FZ_RESULT_SUCCESS;
}

fz_result_t 
fz_lock_destroy(fz_lock_t **lock)
{
    if (pthread_mutex_destroy(*lock) != 0 && errno != EINVAL) {
        return FZ_RESULT_LOCK_ERROR;
    }
    free(*lock);
    *lock = NULL;
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_lock_acquire(fz_lock_t *lock)
{
    fz_result_t result;
    if ((result = pthread_mutex_lock(lock)) != 0) {
        return FZ_RESULT_LOCK_ERROR;
    }
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_lock_release(fz_lock_t *lock)
{
    fz_result_t result;
    if ((result = pthread_mutex_unlock(lock)) != 0) {
        return FZ_RESULT_LOCK_ERROR;
    }
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_hash(
        char *buffer,
        void *data,
        fz_uint_t size)
{
    fz_uint_t i;
    unsigned char digest[16];
    MD5_CTX context;
    MD5Init(&context);
    MD5Update(&context, (unsigned char*)data, size);
    MD5Final(digest, &context);

    memset(buffer, 0, FZ_HASH_SIZE);
    for (i = 0; i < 16 && i < FZ_HASH_SIZE/2; ++i) {
        sprintf(buffer + (i*2), "%02x", digest[i]);
    }
    
    return FZ_RESULT_SUCCESS;
}

const char*
fz_result_string(fz_result_t result)
{
    if (result == FZ_RESULT_SUCCESS) {
        return "success";
    } else if (result == FZ_RESULT_NOT_IMPLEMENTED) {
        return "not implemented";
    } else if (result == FZ_RESULT_MALLOC_ERROR) {
        return "memory allocation error";
    } else if (result == FZ_RESULT_LOCK_ERROR) {
        return "lock error";
    } else if (result == FZ_RESULT_IOOB_ERROR) {
        return "index out-of-bounds error";
    } else if (result == FZ_RESULT_INVALID_ARG) {
        return "invalid argument";
    }
    return "unknown result";
}
