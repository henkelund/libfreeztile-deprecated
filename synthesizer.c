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

#include "synthesizer.h"
#include "freeztile.h"
#include "producer/note.h"
#include "producer/oscillator.h"
#include "form.h"

// ### PRIVATE ###

static
fz_ptr_t
_fz_synthesizer_construct(
                          const fz_ptr_t  self,
                          va_list        *args)
{
    (void) args;
    fz_synthesizer_t *_self = (fz_synthesizer_t*) self;
    _self->ob               = fz_list_new(fz_amp_t);
    _self->note_pool        = fz_list_new_flags(fz_note_t*, FZ_LIST_FLAG_RETAIN);
    _self->active_notes     = fz_list_new_flags(fz_note_t*, FZ_LIST_FLAG_RETAIN);
    _self->sample_rate      = FZ_SAMPLE_RATE;
    _self->voice            = fz_list_new_flags(fz_oscdesc_t*, FZ_LIST_FLAG_RETAIN);
    _self->envdesc          = fz_new(fz_envdesc);
    _self->flags            = FZ_NOTE_STEAL_POLICY_NOSTEAL;

    // Add an oscillator to the note voice with a sample form
    fz_oscdesc_t *osc = fz_new(fz_oscdesc);
    fz_list_append(_self->voice, &osc);
    fz_release(osc);
    fz_list_t *multicurve = fz_list_new(fz_mccurve_t);
    fz_mccurve_t mccurve = {
      .form  = NULL,
      .curve = DEFAULT_CURVE,
      .share = 0.5
    };
    mccurve.curve.start = 1.f;
    mccurve.curve.a.x = 0.5f;
    mccurve.curve.a.y = 1.f;
    mccurve.curve.b.x = 0.5f;
    mccurve.curve.b.y = -1.f;
    mccurve.curve.end = -1.f;
    fz_list_append(multicurve, &mccurve);
    fz_mccurve_t mccurve2 = {
      .form  = NULL,
      .curve = DEFAULT_CURVE,
      .share = 0.5
    };
    mccurve2.curve.start = -1.f;
    mccurve2.curve.a.x = 0.5f;
    mccurve2.curve.a.y = -1.f;
    mccurve2.curve.b.x = 0.5f;
    mccurve2.curve.b.y = 1.f;
    mccurve2.curve.end = 1.f;
    fz_list_append(multicurve, &mccurve2);
    fz_list_clear(osc->form->template, 1024);
    fz_multicurve_render(multicurve, osc->form);
    // Now the curve is rasterized to the oscillator form template
    // so we can free the curve descriptors
    fz_size_t i = 0;
    for (; i < fz_list_size(multicurve); ++i) {
        // free sub curve cache
        fz_release(fz_list_ref(multicurve, i, fz_mccurve_t)->form);
    }
    fz_release(multicurve);

    // Sample amplifier adsr
    fz_envelope_set_adsr(_self->envdesc, 1., 44100, .4, 25000, .9, 40000, 50000);
    fz_synthesizer_set_polyphony(_self, FZ_POLYPHONY_LEVEL);

    return _self;
}

static
fz_ptr_t
_fz_synthesizer_destruct(const fz_ptr_t self)
{
    fz_synthesizer_t *_self = (fz_synthesizer_t*) self;

    fz_release(_self->ob);
    fz_release(_self->note_pool);
    fz_release(_self->active_notes);
    fz_release(_self->voice);
    fz_release(_self->envdesc);

    return _self;
}

static const fz_object_t _fz_synthesizer = {
    sizeof (fz_synthesizer_t),
    _fz_synthesizer_construct,
    _fz_synthesizer_destruct,
    NULL,
    NULL
};

// ### PUBLIC ###

const fz_ptr_t fz_synthesizer = (const fz_ptr_t) &_fz_synthesizer;

fz_list_t*
fz_synthesizer_output(
                      fz_synthesizer_t *synth,
                      fz_uint_t         num_frames)
{
    fz_size_t  i;
    fz_note_t *note;

    fz_list_clear(synth->ob, num_frames);

    if (!fz_list_size(synth->active_notes) > 0) {
        return synth->ob;
    }

    for (i = 0; i < fz_list_size(synth->active_notes); ++i) {
        note              = fz_list_val(synth->active_notes, i, fz_note_t*);
        note->voice       = synth->voice;
        note->sample_rate = synth->sample_rate;
        fz_produce(note, synth->ob);

        // Put back in pool if done playing
        if (!fz_note_is_active(note)) {
            // Append before remove or note will be freed (FZ_LIST_FLAG_RETAIN)
            fz_list_append(synth->note_pool, &note);
            fz_list_remove(synth->active_notes, i);
            --i;
        }
    }

    return synth->ob;
}

fz_result_t
fz_synthesizer_set_polyphony(
                             fz_synthesizer_t *synth,
                             fz_uint_t         level)
{
    fz_uint_t      current_level = fz_synthesizer_get_polyphony(synth);
    fz_note_t     *note;
    fz_envelope_t *envelope;

    for (; current_level < level; ++current_level) {
        if (current_level == 0) {
            note = fz_new(fz_note);
            note->voice = synth->voice;
            envelope = fz_map_val(note->envelopes, FZ_AMPLIFIER_KEY, fz_envelope_t*);
            if (envelope != NULL) {
                envelope->descriptor = fz_retain(synth->envdesc);
            }
        } else {
            note = fz_clone(fz_list_val(
                                (fz_list_size(synth->note_pool) > 0 ?
                                        synth->note_pool
                                        :
                                        synth->active_notes),
                                0, fz_note_t*));
        }
        fz_list_append(synth->note_pool, &note);
        fz_release(note);
    }

    for (; current_level > level; --current_level) {
        if (fz_list_size(synth->note_pool) > 0) {
            fz_list_remove(synth->note_pool, fz_list_size(synth->note_pool) - 1);
        } else {
            fz_list_remove(
                    synth->active_notes, fz_list_size(synth->active_notes) - 1);
        }
    }

    return FZ_RESULT_SUCCESS;
}

fz_uint_t
fz_synthesizer_get_polyphony(fz_synthesizer_t *synth)
{
    return fz_list_size(synth->note_pool) + fz_list_size(synth->active_notes);
}

fz_note_t*
fz_synthesizer_play(
                    fz_synthesizer_t *synth,
                    fz_real_t         frequency,
                    fz_amp_t          amplitude)
{
    fz_note_t *note = NULL;

    if (fz_list_size(synth->note_pool) > 0) {

        note = fz_list_val(synth->note_pool,
                                fz_list_size(synth->note_pool) - 1, fz_note_t*);
        fz_retain(note);
        fz_list_remove(synth->note_pool, fz_list_size(synth->note_pool) - 1);

    } else {

        if (synth->flags & FZ_NOTE_STEAL_POLICY_FIFO &&
                fz_list_size(synth->active_notes) > 0) {

            note = fz_list_val(synth->active_notes, 0, fz_note_t*);
            fz_retain(note);
            fz_list_remove(synth->active_notes, 0);

        } else if (synth->flags & FZ_NOTE_STEAL_POLICY_QUIETEST) {

            // TODO: Implement when ADSR is in place

        } else if (synth->flags & FZ_NOTE_STEAL_POLICY_EXPAND) {

            fz_synthesizer_set_polyphony(
                    synth, fz_synthesizer_get_polyphony(synth) + 1);

            return fz_synthesizer_play(synth, frequency, amplitude);
        }
    }

    if (note != NULL) {
        note->freq = frequency;
        fz_note_start(note, amplitude);
        fz_list_append(synth->active_notes, &note);
        fz_release(note);
    }

    return note;
}
