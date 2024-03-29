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

#include <stdio.h>

#include "adapter.h"
#include "../freeztile.h"

// ### PRIVATE ###

static
fz_ptr_t
_fz_playback_adapter_construct(
                                const fz_ptr_t  self,
                                va_list        *args)
{
    fz_playback_adapter_t *_self = (fz_playback_adapter_t*) self;
    _self->stopped               = TRUE;
    _self->_play_callback        = NULL;
    _self->synthesizer           = va_arg(*args, fz_synthesizer_t*);

    pthread_mutex_init(&_self->mutex, NULL);

    if (_self->synthesizer != NULL) {
        fz_retain(_self->synthesizer);
    }

    return _self;
}

static
fz_ptr_t
_fz_playback_adapter_destruct(const fz_ptr_t  self)
{
    fz_playback_adapter_t *_self = (fz_playback_adapter_t*) self;
    fz_playback_adapter_stop(_self);

    if (_self->synthesizer != NULL) {
        fz_release(_self->synthesizer);
    }

    pthread_mutex_destroy(&_self->mutex);

    return _self;
}

static
void*
_fz_playback_adapter_pthread_callback(void *arg)
{
    fz_playback_adapter_t *adapter = (fz_playback_adapter_t*) arg;

    while (!adapter->stopped) {
        if (adapter->_play_callback(adapter) <= 0) {
            fz_playback_adapter_stop(adapter);
            FZ_IF_DEBUG( fprintf(stderr, "Playback adapter failed\n"); )
        }
    }

    return NULL;
}

static const fz_object_t _fz_playback_adapter = {
    sizeof (fz_playback_adapter_t),
    _fz_playback_adapter_construct,
    _fz_playback_adapter_destruct,
    NULL,
    NULL
};

// ### PUBLIC ###

const fz_ptr_t fz_playback_adapter = (const fz_ptr_t) &_fz_playback_adapter;

fz_result_t
fz_playback_adapter_start(fz_playback_adapter_t *adapter)
{
    if (adapter->stopped) {
        adapter->stopped = FALSE;
        pthread_attr_init(&adapter->thread_attr);
        pthread_create(
                &adapter->thread,
                &adapter->thread_attr,
                _fz_playback_adapter_pthread_callback,
                (void*) adapter);
    }
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_playback_adapter_stop(fz_playback_adapter_t *adapter)
{
    if (!adapter->stopped) {
        adapter->stopped = TRUE;
        pthread_join(adapter->thread, NULL);
    }
    return FZ_RESULT_SUCCESS;
}

fz_int_t
fz_playback_adapter_lock(fz_playback_adapter_t *adapter)
{
    return pthread_mutex_lock(&adapter->mutex);
}

fz_int_t
fz_playback_adapter_unlock(fz_playback_adapter_t *adapter)
{
    return pthread_mutex_unlock(&adapter->mutex);
}
