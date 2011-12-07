#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "freeztile.h"

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
