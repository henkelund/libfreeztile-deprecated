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

#include "producer.h"

#define _FZ_PRODUCER_FLAG_NONE         0
#define _FZ_PRODUCER_FLAG_INITIALIZED (1 << 0)

typedef struct fz_producer_private_s {
    fz_flags_t    flags;
    fz_int_t    (*produce)(fz_producer_t *self, fz_list_t *buffer);
    fz_result_t (*prepare)(fz_producer_t *self, fz_size_t  size);
} fz_producer_private_t;

static
fz_ptr_t
_fz_producer_construct(fz_ptr_t  self,
                       va_list  *args)
{
    (void) args;
    fz_producer_t *_self = (fz_producer_t*) self;
    fz_producer_private_t *private = (fz_producer_private_t*)
                                        fz_malloc(sizeof (fz_producer_private_t));
    private->flags   = _FZ_PRODUCER_FLAG_NONE;
    private->prepare = NULL;
    private->produce = NULL;
    *((const fz_producer_private_t**) &_self->_private) = private;

    return _self;
}

static
fz_ptr_t
_fz_producer_destruct(fz_ptr_t  self)
{
    fz_producer_t *_self = (fz_producer_t*) self;
    fz_free(_self->_private);
    return _self;
}

static const fz_object_t _fz_producer = {
    sizeof (fz_producer_t),
    _fz_producer_construct,
    _fz_producer_destruct,
    NULL,
    NULL
};

const fz_ptr_t fz_producer = (const fz_ptr_t) &_fz_producer;

fz_result_t
_fz_producer_init(
                  fz_ptr_t producer,
                  fz_ptr_t produce_fnc,
                  fz_ptr_t prepare_fnc)
{
    fz_producer_t *_producer = (fz_producer_t*) producer;
    fz_producer_private_t *private = (fz_producer_private_t*) _producer->_private;
    if (private->flags & _FZ_PRODUCER_FLAG_INITIALIZED) {
        return FZ_RESULT_SUCCESS;
    }
    private->produce  = produce_fnc;
    private->prepare  = prepare_fnc;
    private->flags   |= _FZ_PRODUCER_FLAG_INITIALIZED;
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_produce(
           fz_ptr_t   producer,
           fz_list_t *buffer)
{
    fz_producer_t *_producer = (fz_producer_t*) producer;
    fz_producer_private_t *private;
    if (!_producer) {
        return -FZ_RESULT_INVALID_ARG;
    }
    private = (fz_producer_private_t*) _producer->_private;
    if (private != NULL &&
            private->produce != NULL &&
                (private->prepare == NULL ||
                    FZ_RESULT_SUCCESS ==
                        private->prepare(_producer, fz_list_size(buffer)))) {
        return private->produce(_producer, buffer);
    }
    return 0;
}
