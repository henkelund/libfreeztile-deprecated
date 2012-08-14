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
#include "../freeztile.h"
#include "../util/list.h"
#include "oscillator.h"
#include "../synthesizer.h"

// ### PRIVATE ###

#define FZ_HALFNOTE_DIST 1.059463094 // pow(2, 1.0/12)

static
fz_result_t
_fz_note_sync(fz_note_t *note)
{
    fz_oscillator_t *oscillator;
    fz_uint_t        i;
    if (note->voice == NULL) {
        return FZ_RESULT_SUCCESS;
    }
    for (i = 0; i < note->voice->size; ++i) {
        if (i == note->oscillators->size) {
            oscillator = fz_new(fz_oscillator);
            fz_list_append(note->oscillators, &oscillator);
            fz_release(oscillator);
        }
        fz_list_val(note->oscillators, i, fz_oscillator_t*)->descriptor =
                                        fz_list_val(note->voice, i, fz_oscdesc_t*);
    }
    return FZ_RESULT_SUCCESS;
}

static
fz_result_t
_fz_note_produce(
                 fz_producer_t *self,
                 fz_list_t     *buffer)
{
    fz_note_t       *_self = (fz_note_t*) self;
    fz_oscillator_t *oscillator;
    fz_uint_t        i;
    fz_envelope_t   *envelope;
    fz_filter_t     *filter;
    fz_result_t      result = FZ_RESULT_SUCCESS;

    if (_self->voice && (result = _fz_note_sync(_self)) != FZ_RESULT_SUCCESS) {
        return result;
    } else if (_self->freq > 0) {

        fz_list_clear(_self->ob, buffer->size);

        for (i = 0; i < _self->oscillators->size; ++i) {
            oscillator              = fz_list_val(
                                            _self->oscillators, i, fz_oscillator_t*);
            oscillator->sample_rate = _self->sample_rate;
            oscillator->freq        = _self->freq;
            fz_produce(oscillator, _self->ob);
        }

        for (i = 0; i < fz_map_size(_self->filters); ++i) {

            filter = fz_map_ival(_self->filters, i, fz_filter_t*);

            envelope = fz_map_val(_self->envelopes,
                            fz_map_ikey(_self->filters, i), fz_envelope_t*);
            if (envelope) {
                fz_list_clear(_self->env_ob, _self->ob->size);
                fz_produce(envelope, _self->env_ob);
                filter->regulator = _self->env_ob;
            } else {
                filter->regulator = NULL;
            }

            fz_filtrate(fz_map_ival(_self->filters, i, fz_filter_t*), _self->ob);
        }

        for (i = 0; i < buffer->size; ++i) {
            fz_list_val(buffer, i, fz_amp_t) +=
                    fz_list_val(_self->ob, i, fz_amp_t);
        }
    }

    return result;
}

static
fz_ptr_t
_fz_note_construct(
                   const fz_ptr_t  self,
                   va_list        *args)
{
    (void) args;
    fz_note_t *_self   = (fz_note_t*) self;
    _self->oscillators = fz_list_new_flags(fz_oscillator_t*, FZ_LIST_FLAG_RETAIN);
    _self->voice       = NULL;
    _self->freq        = 0;
    _self->sample_rate = FZ_SAMPLE_RATE;
    _self->flags       = FZ_NOTE_FLAG_NONE;
    _self->ob          = fz_list_new(fz_amp_t);
    _self->envelopes   = fz_map_new_flags(fz_envelope_t*,
                                FZ_MAP_FLAG_RETAIN | FZ_MAP_FLAG_ITERABLE);
    _self->env_ob      = fz_list_new(fz_amp_t);
    _self->filters     = fz_map_new_flags(fz_filter_t*,
                                FZ_MAP_FLAG_RETAIN | FZ_MAP_FLAG_ITERABLE);

    fz_envelope_t *env = fz_new(fz_envelope, NULL);
    fz_map_set_noretain(_self->envelopes, FZ_AMPLIFIER_KEY, &env);

    fz_amplifier_t *amp = fz_new(fz_amplifier);
    fz_map_set_noretain(_self->filters, FZ_AMPLIFIER_KEY, &amp);

    ((fz_producer_t*) self)->produce = _fz_note_produce;

    return _self;
}

static
fz_ptr_t
_fz_note_destruct(fz_ptr_t self)
{
    fz_note_t *_self = (fz_note_t*) self;
    fz_release(_self->oscillators);
    fz_release(_self->ob);
    fz_release(_self->envelopes);
    fz_release(_self->env_ob);
    fz_release(_self->filters);
    // synthesizer is responsible for _self->voice

    return _self;
}

static
fz_ptr_t
_fz_note_clone(const fz_ptr_t self)
{
    fz_note_t *_self = (fz_note_t*) self;
    fz_note_t *clone = fz_new(fz_note);

    // Do malloc stuff. Better to do it here than "just in time" in produce callback
    fz_list_grow(clone->oscillators, _self->oscillators->size);
    fz_list_grow(clone->ob,          _self->ob->size);
    fz_list_grow(clone->env_ob,      _self->env_ob->size);

    clone->voice       = _self->voice;
    clone->sample_rate = _self->sample_rate;
    // clone->flags = (_self->flags &= ~FZ_NOTE_FLAG_ACTIVE);
    // ^-- Might be a good idea if it makes sense for other flags
    _fz_note_sync(clone);

    fz_envelope_t *clone_ampenv = fz_map_val(clone->envelopes,
                                    FZ_AMPLIFIER_KEY, fz_envelope_t*);
    fz_envelope_t *_self_ampenv = fz_map_val(_self->envelopes,
                                    FZ_AMPLIFIER_KEY, fz_envelope_t*);

    if (clone_ampenv && _self_ampenv && _self_ampenv->descriptor != NULL) {
        clone_ampenv->descriptor = fz_retain(_self_ampenv->descriptor);
    }

    return clone;
}

static const fz_object_t _fz_note = {
    sizeof (fz_note_t),
    _fz_note_construct,
    _fz_note_destruct,
    _fz_note_clone,
    NULL
};

// ### PUBLIC ###

const fz_ptr_t fz_note = (const fz_ptr_t) &_fz_note;

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
    fz_envelope_t *envelope = fz_map_get(note->envelopes, FZ_AMPLIFIER_KEY);
    if (envelope) {
        if (envelope->state == FZ_ENV_SILENT) {
            note->flags &= ~FZ_NOTE_FLAG_ACTIVE;
            return FALSE;
        }
        note->flags |= FZ_NOTE_FLAG_ACTIVE;
        return TRUE;
    }
    return (note->flags & FZ_NOTE_FLAG_ACTIVE) ? TRUE : FALSE;
}

fz_result_t
fz_note_start(
              fz_note_t *note,
              fz_amp_t   amplitude)
{
    fz_amplifier_t *amplifier =
            fz_map_val(note->filters, FZ_AMPLIFIER_KEY, fz_amplifier_t*);
    fz_envelope_t  *envelope  =
            fz_map_val(note->envelopes, FZ_AMPLIFIER_KEY, fz_envelope_t*);

    if (amplifier != NULL) {
        amplifier->level = amplitude;
    }

    if (envelope != NULL) {
        fz_envelope_attack(envelope);
    }

    note->flags |= FZ_NOTE_FLAG_ACTIVE;

    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_note_stop(fz_note_t *note)
{
    fz_envelope_t *envelope =
            fz_map_val(note->envelopes, FZ_AMPLIFIER_KEY, fz_envelope_t*);
    if (envelope != NULL && envelope->state != FZ_ENV_SILENT) {
        if (envelope->state != FZ_ENV_RELEASE) {
            fz_envelope_release(envelope);
        }
    } else {
        note->flags &= ~FZ_NOTE_FLAG_ACTIVE;
    }
    return FZ_RESULT_SUCCESS;
}
