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
#include "note.h"
#include "oscillator.h"
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
    _self->voice            = fz_list_new_flags(fz_osc_t*, FZ_LIST_FLAG_RETAIN);
    _self->flags            = FZ_NOTE_STEAL_POLICY_NOSTEAL;

    // Add an oscillator to the note voice with a sample form
    fz_list_append_release(_self->voice, fz_new(fz_osc), osc, fz_osc_t*);
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
    for (; i < multicurve->size; ++i) {
        // free sub curve cache
        fz_free(fz_list_ref(multicurve, i, fz_mccurve_t)->form);
    }
    fz_free(multicurve);

    fz_synthesizer_set_polyphony(_self, FZ_POLYPHONY_LEVEL);

    return _self;
}

static
fz_ptr_t
_fz_synthesizer_destruct(const fz_ptr_t self)
{
    fz_synthesizer_t *_self = (fz_synthesizer_t*) self;

    fz_free(_self->ob);
    fz_free(_self->note_pool);
    fz_free(_self->active_notes);
    fz_free(_self->voice);

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
    fz_amp_t   amp;

    fz_list_clear(synth->ob, num_frames);

    if (!synth->active_notes->size > 0) {
        return synth->ob;
    }

    amp = 1.0/synth->active_notes->size ;

    for (i = 0; i < synth->active_notes->size; ++i) {
        note        = fz_list_val(synth->active_notes, i, fz_note_t*);
        note->voice = synth->voice;
        fz_note_apply(note, synth->ob, amp, synth->sample_rate);

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
    fz_uint_t current_level = synth->note_pool->size + synth->active_notes->size;

    for (; current_level < level; ++current_level) {
        fz_list_append_release(synth->note_pool, fz_new(fz_note), note, fz_note_t*);
        note->voice = synth->voice;
    }

    for (; current_level > level; --current_level) {
        if (synth->note_pool->size > 0) {
            fz_list_remove(synth->note_pool, synth->note_pool->size - 1);
        } else {
            fz_list_remove(synth->active_notes, synth->active_notes->size - 1);
        }
    }

    return FZ_RESULT_SUCCESS;
}

fz_note_t*
fz_synthesizer_play(
                    fz_synthesizer_t *synth,
                    fz_real_t         frequency)
{
    fz_note_t *note = NULL;

    if (synth->note_pool->size > 0) {

        note = fz_list_val(synth->note_pool, synth->note_pool->size - 1, fz_note_t*);
        fz_retain(note);
        fz_list_remove(synth->note_pool, synth->note_pool->size - 1);

    } else if (synth->active_notes->size > 0) {

        if (synth->flags & FZ_NOTE_STEAL_POLICY_FIFO) {

            note = fz_list_val(synth->active_notes, 0, fz_note_t*);
            fz_retain(note);
            fz_list_remove(synth->active_notes, 0);

        } else if (synth->flags & FZ_NOTE_STEAL_POLICY_QUIETEST) {
            // TODO: Implement when ADSR is in place
        }
    }

    if (note != NULL) {
        note->freq      = frequency;
        note->is_active = TRUE;
        fz_list_append(synth->active_notes, &note);
        fz_free(note);
    }

    return note;
}
