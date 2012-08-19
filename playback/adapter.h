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

#ifndef _FZ_PLAYBACK_ADAPTER_H_
#define _FZ_PLAYBACK_ADAPTER_H_

#include <pthread.h>
#include "../freeztile.h"
#include "../synthesizer.h"

typedef struct fz_playback_adapter_s {
    const fz_ptr_t     _class;
    fz_synthesizer_t  *synthesizer;
    pthread_t          thread;
    pthread_attr_t     thread_attr;
    pthread_mutex_t    mutex;
    fz_bool_t          stopped;
    fz_int_t         (*_play_callback)(struct fz_playback_adapter_s *self);
} fz_playback_adapter_t;

/**
 * Start playback server
 *
 * @param  fz_playback_adapter_t *adapter
 * @return fz_result_t
 */
fz_result_t
fz_playback_adapter_start(fz_playback_adapter_t *adapter);

/**
 * Stop playback server
 *
 * @param  fz_playback_adapter_t *adapter
 * @return fz_result_t
 */
fz_result_t
fz_playback_adapter_stop(fz_playback_adapter_t *adapter);

/**
 * Block the playback thread
 *
 * @param  fz_playback_adapter_t *adapter
 * @return fz_int_t
 */
fz_int_t
fz_playback_adapter_lock(fz_playback_adapter_t *adapter);

/**
 * Unblock the playback thread
 *
 * @param  fz_playback_adapter_t *adapter
 * @return fz_int_t
 */
fz_int_t
fz_playback_adapter_unlock(fz_playback_adapter_t *adapter);

const fz_ptr_t fz_playback_adapter;

#endif // _FZ_PLAYBACK_ADAPTER_H_
