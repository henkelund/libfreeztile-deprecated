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

#ifndef _FZ_PLAYBACK_ALSA_H_
#define _FZ_PLAYBACK_ALSA_H_

#include <alsa/asoundlib.h>
#include "adapter.h"

typedef struct {
    const fz_playback_adapter_t  _super;
    snd_pcm_t                   *playback_handle;
    snd_pcm_hw_params_t         *hw_params;
    snd_pcm_sw_params_t         *sw_params;
    fz_size_t                    buffer_size;
} fz_playback_alsa_t;

const fz_ptr_t fz_playback_alsa;

#endif // _FZ_PLAYBACK_ALSA_H_
