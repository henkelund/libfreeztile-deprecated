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

#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "note.h"
#include "freeztile.h"
#include "util/list.h"
#include "oscillator.h"

// ### PRIVATE ###

#define FZ_HALFNOTE_DIST 1.059463094 // pow(2, 1.0/12)

static
fz_ptr_t
_fz_note_construct(
                   const fz_ptr_t  self,
                   va_list        *args)
{
    (void) args;
    fz_note_t *_self = (fz_note_t*) self;
    _self->octxs     = fz_list_new(fz_octx_t);
    _self->voice     = NULL;
    _self->freq      = 0;
    _self->is_active = FALSE;
    _self->buffer    = fz_list_new(fz_amp_t);
    _self->envelopes = fz_map_new_flags(fz_envelope_t*,
                                FZ_MAP_FLAG_RETAIN | FZ_MAP_FLAG_ITERABLE);
    _self->filters   = fz_map_new_flags(fz_filter_t*,
                                FZ_MAP_FLAG_RETAIN | FZ_MAP_FLAG_ITERABLE);

    fz_envelope_t *env = fz_new(fz_envelope, NULL);
    fz_map_set_noretain(_self->envelopes, FZ_AMPLIFIER_KEY, &env);

    fz_amplifier_t *amp = fz_new(fz_amplifier);
    ((fz_filter_t*) amp)->regulator = (fz_producer_t*) env;
    fz_map_set_noretain(_self->filters, FZ_AMPLIFIER_KEY, &amp);

    return _self;
}

static
fz_ptr_t
_fz_note_destruct(fz_ptr_t self)
{
    fz_note_t *_self = (fz_note_t*) self;
    fz_uint_t i;
    for (i = 0; i < _self->octxs->size; ++i) {
        fz_free(fz_list_ref(_self->octxs, i, fz_octx_t)->framebuf);
    }
    fz_free(_self->octxs);
    fz_free(_self->buffer);
    fz_free(_self->envelopes);
    fz_free(_self->filters);
    // synthesizer is responsible for _self->voice

    return _self;
}

static fz_octx_t _fz_default_octx = {
    .osc      = NULL,
    .frame    = 0,
    .framebuf = NULL,
    .freq     = 0,
    .amp      = 1
};

static
fz_result_t
_fz_note_sync(fz_note_t *note)
{
    fz_uint_t i;
    for (i = 0; i < note->voice->size; ++i) {
        if (i == note->octxs->size) {
            fz_list_append(note->octxs, &_fz_default_octx);
            fz_list_ref(note->octxs, i, fz_octx_t)->framebuf =
                    fz_list_new(fz_frame_t);
        }
        fz_list_ref(note->octxs, i, fz_octx_t)->osc =
                fz_list_val(note->voice, i, fz_osc_t*);
    }
    return FZ_RESULT_SUCCESS;
}

static const fz_object_t _fz_note = {
    sizeof (fz_note_t),
    _fz_note_construct,
    _fz_note_destruct,
    NULL,
    NULL
};

// ### PUBLIC ###

const fz_ptr_t fz_note = (const fz_ptr_t) &_fz_note;

fz_result_t
fz_note_apply(
              fz_note_t *note,
              fz_list_t *output,
              fz_amp_t   amplitude,
              fz_uint_t  sample_rate)
{
    fz_octx_t   *ctx;
    fz_uint_t    i;
    fz_result_t  result = FZ_RESULT_SUCCESS;

    if (note->voice && (result = _fz_note_sync(note)) != FZ_RESULT_SUCCESS) {
        return result;
    } else if (note->freq > 0) {

        fz_list_clear(note->buffer, output->size);

        for (i = 0; i < note->octxs->size; ++i) {
            ctx              = fz_list_ref(note->octxs, i, fz_octx_t);
            ctx->amp         = amplitude;
            ctx->sample_rate = sample_rate;
            ctx->freq        = note->freq;
            fz_list_clear(ctx->framebuf, note->buffer->size);
            fz_oscillator_apply(ctx, note->buffer);
        }

        for (i = 0; i < fz_map_size(note->filters); ++i) {
            fz_filtrate(fz_map_ival(note->filters, i, fz_filter_t*), note->buffer);
        }

        for (i = 0; i < output->size; ++i) {
            fz_list_val(output, i, fz_amp_t) +=
                    fz_list_val(note->buffer, i, fz_amp_t);
        }
    }

    return result;
}

fz_real_t
fz_note_parse_frequency(const fz_char_t *name)
{
    static const fz_char_t map[] = {'c',  '\0', 'd',  '\0', 'e',  'f',
                                    '\0', 'g',  '\0', 'a',  '\0', 'b'} ;
    fz_int_t  length             = strlen(name)                        ,
              i                                                        ,
              pos                = 0                                   ,
              octave             = 4                                   ,
              offset                                                   ;
    fz_char_t note                                                     ;

    if (length == 0) {
        return -1;
    }

    for (; pos < length && isspace(name[pos]); ++pos);

    if (pos == length) {
        return -1;
    }

    note = tolower(name[pos]);

    for (i = 0; i < 12; ++i) {
        if (note == map[i]) {
            offset = i - 9;
            break;
        } else if (i == 11) {
            return -1;
        }
    }

    for (++pos; pos < length; ++pos) {
        if (tolower(name[pos]) == 'b') {
            --offset;
        } else if (name[pos] == '#') {
            ++offset;
        } else {
            break;
        }
    }

    octave = (pos < length ? atoi(name + pos) : octave) - 4;

    return 440.0*pow(FZ_HALFNOTE_DIST, offset + 12*octave);
}

fz_bool_t
fz_note_is_active(fz_note_t *note)
{
    // This is a temp implementation until ADSR is in place
    return note->is_active;
}
