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
#include <limits.h>
#include "alsa.h"
#include "../freeztile.h"
#include "../util/list.h"

// ### PRIVATE ###

static
fz_int_t
_fz_playback_alsa_play(fz_playback_adapter_t *self)
{
    fz_playback_alsa_t *_self      = (fz_playback_alsa_t*) self ;
    fz_synthesizer_t   *synth      = self->synthesizer          ;
    fz_list_t          *output                                  ;
    fz_int_t            err        = 0                          ;
    snd_pcm_sframes_t   num_frames                              ;

    if (!self || !synth) {
        return 0;
    }

    if ((err = snd_pcm_wait(_self->playback_handle, 1000)) < 0) {
        fprintf(stderr, "poll failed (%s)\n", strerror(errno));
        return err;
    }

    if ((num_frames = snd_pcm_avail_update(_self->playback_handle)) < 0) {
        if (num_frames == -EPIPE) {
            fprintf(stderr, "an xrun occured\n");
        } else {
            fprintf(stderr, "unknown ALSA avail update return value (%d)\n",
                    (int) num_frames);
        }
        return num_frames;
    }

    num_frames = num_frames > _self->buffer_size ? _self->buffer_size : num_frames;

    output = fz_retain(fz_synthesizer_output(synth, num_frames));

    if (output->size != num_frames) {
        fz_free(output);
        return 0;
    }

    if ((err = snd_pcm_writei(
            _self->playback_handle, output->items, num_frames)) < 0) {
        fprintf(stderr, "write failed (%s)\n", snd_strerror(err));
    }

    fz_free(output);

    return err;
}

static
fz_int_t
_fz_playback_alsa_init_hw_params(fz_playback_alsa_t *self)
{
    fz_int_t             err       = 0                                     ;
    fz_uint_t            rate      = self->_super.synthesizer->sample_rate ;
    snd_pcm_hw_params_t *hw_params                                         ;

    if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
        return err;
    }

    if ((err = snd_pcm_hw_params_any(self->playback_handle, hw_params)) < 0) {
        snd_pcm_hw_params_free(hw_params);
        return err;
    }

    if ((err = snd_pcm_hw_params_set_access(
            self->playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        snd_pcm_hw_params_free(hw_params);
        return err;
    }

    if ((err = snd_pcm_hw_params_set_format(
            self->playback_handle, hw_params, SND_PCM_FORMAT_FLOAT_LE)) < 0) {
        snd_pcm_hw_params_free(hw_params);
        return err;
    }

    if ((err = snd_pcm_hw_params_set_rate(
            self->playback_handle, hw_params, rate, 0)) < 0) {
        snd_pcm_hw_params_free(hw_params);
        return err;
    }

    if ((err = snd_pcm_hw_params_set_channels(
            self->playback_handle, hw_params, 1)) < 0) {
        snd_pcm_hw_params_free(hw_params);
        return err;
    }

    if ((err = snd_pcm_hw_params(self->playback_handle, hw_params)) < 0) {
        snd_pcm_hw_params_free(hw_params);
        return err;
    }

    snd_pcm_hw_params_free(hw_params);
    return err;
}

static
fz_int_t
_fz_playback_alsa_init_sw_params(fz_playback_alsa_t *self)
{
    fz_int_t             err       = 0 ;
    snd_pcm_sw_params_t *sw_params     ;

    if ((err = snd_pcm_sw_params_malloc(&sw_params)) < 0) {
        return err;
    }

    if ((err = snd_pcm_sw_params_current(self->playback_handle, sw_params)) < 0) {
        snd_pcm_sw_params_free(sw_params);
        return err;
    }

    if ((err = snd_pcm_sw_params_set_avail_min(
            self->playback_handle, sw_params, self->buffer_size)) < 0) {
        snd_pcm_sw_params_free(sw_params);
        return err;
    }
    if ((err = snd_pcm_sw_params_set_start_threshold(
            self->playback_handle, sw_params, 0U)) < 0) {
        snd_pcm_sw_params_free(sw_params);
        return err;
    }
    if ((err = snd_pcm_sw_params(self->playback_handle, sw_params)) < 0) {
        snd_pcm_sw_params_free(sw_params);
        return err;
    }

    if ((err = snd_pcm_prepare(self->playback_handle)) < 0) {
        snd_pcm_sw_params_free(sw_params);
        return err;
    }

    snd_pcm_sw_params_free(sw_params);
    return err;
}

static
fz_ptr_t
_fz_playback_alsa_construct(
                            const fz_ptr_t  self,
                            va_list        *args)
{
    fz_int_t err;
    fz_playback_alsa_t *_self =
            ((const fz_object_t*) fz_playback_adapter)->construct(self, args);
    ((fz_playback_adapter_t*) _self)->_play_callback = _fz_playback_alsa_play;

    if ((err = snd_pcm_open(
            &_self->playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf(stderr, "cannot open audio device (%s)\n", snd_strerror(err));
        exit(err);
    }
    if ((err = _fz_playback_alsa_init_hw_params(_self)) < 0) {
        fprintf(stderr, "hardware param error (%s)\n", snd_strerror(err));
        exit(err);
    }
    if ((err = _fz_playback_alsa_init_sw_params(_self)) < 0) {
        fprintf(stderr, "software param error (%s)\n", snd_strerror(err));
        exit(err);
    }

    _self->buffer_size = va_arg(*args, fz_size_t);

    return _self;
}

static
fz_ptr_t
_fz_playback_alsa_destruct(fz_ptr_t self)
{
    fz_playback_alsa_t *_self =
            ((const fz_object_t*) fz_playback_adapter)->destruct(self);
    snd_pcm_drain(_self->playback_handle);
    snd_pcm_close(_self->playback_handle);
    return _self;
}

static const fz_object_t _fz_playback_alsa = {
    sizeof (fz_playback_alsa_t),
    _fz_playback_alsa_construct,
    _fz_playback_alsa_destruct,
    NULL,
    NULL
};

const fz_ptr_t fz_playback_alsa = (const fz_ptr_t) &_fz_playback_alsa;
