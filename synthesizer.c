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

    /*/// Create example note
    fz_note_t *note = fz_new(fz_note);
    // Use note->voice = fz_retain(voice) to avoid note taking voice ownership
    note->voice = fz_list_new_flags(fz_osc_t*, FZ_LIST_FLAG_RETAIN);
    fz_list_append(_self->note_pool, &note);
    // _self->note_pool is created with FZ_LIST_FLAG_RETAIN so we can free the
    // note immediately to give the note pool ownership
    fz_free(note);
    note = fz_list_val(_self->note_pool, 0, fz_note_t*);*/

    // This macro does the same as the commented out lines above
    fz_list_append_release(_self->note_pool, fz_new(fz_note), note, fz_note_t*);
    note->voice = fz_list_new_flags(fz_osc_t*, FZ_LIST_FLAG_RETAIN);

    // Add an oscillator to the note voice with a sample form
    fz_list_append_release(note->voice, fz_new(fz_osc), osc, fz_osc_t*);
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

    /*#include <stdio.h>
    FILE *file = fopen("form.csv", "w");
    for (i = 0; i < osc->form->template->size; ++i) {
        fprintf(file, "%f\n", fz_list_val(osc->form->template, i, fz_amp_t));
    }
    fclose(file);*/

    return _self;
}

static
fz_ptr_t
_fz_synthesizer_destruct(const fz_ptr_t  self)
{
    fz_synthesizer_t *_self = (fz_synthesizer_t*) self;

    fz_free(_self->ob);
    fz_free(_self->note_pool);

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
    fz_list_clear(synth->ob, num_frames);
    fz_note_apply(fz_list_val(synth->note_pool, 0, fz_note_t*), synth->ob);
    return synth->ob;
}
