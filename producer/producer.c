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

static
fz_ptr_t
_fz_producer_construct(fz_ptr_t  self,
                       va_list  *args)
{
    fz_producer_t *_self        = (fz_producer_t*) self;
    *((void**) &_self->produce) = va_arg(args, void*);
    return _self;
}

static const fz_object_t _fz_producer = {
    sizeof (fz_producer_t),
    _fz_producer_construct,
    NULL,
    NULL,
    NULL
};

const fz_ptr_t fz_producer = (const fz_ptr_t) &_fz_producer;

fz_result_t
fz_produce(
           fz_ptr_t   producer,
           fz_list_t *buffer)
{
    fz_producer_t *_producer = (fz_producer_t*) producer;
    if (!_producer || !_producer->produce) {
        return -FZ_RESULT_INVALID_ARG;
    }

    return _producer->produce(_producer, buffer);
}
