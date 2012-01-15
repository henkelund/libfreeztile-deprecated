/**
 * libfreeztile
 *
 * Copyright (C) 2011 Henrik Hedelund (henke.hedelund@gmail.com)
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
#include "note.h"
#include "freeztile.h"
#include "list.h"

// ### PRIVATE ###

fz_ptr_t
_fz_note_construct(
                   const fz_ptr_t  self,
                   va_list        *args)
{
    (void) args;
    fz_note_t *_self = (fz_note_t*) self;
    _self->octxs     = fz_list_new(fz_octx_t);
    _self->voice     = NULL;
    return _self;
}

fz_ptr_t
_fz_note_destruct(fz_ptr_t self)
{
    fz_note_t *_self = (fz_note_t*) self;
    // if we move osc frame_buf's to ctx we should free those as well
    fz_free(_self->octxs);
    // _self->voice isn't owned by the note
    return _self;
}

fz_octx_t _fz_default_octx = {
    .osc   = NULL,
    .frame = 0,
    .freq  = 0
};

fz_result_t
_fz_note_sync(fz_note_t *note)
{
    fz_uint_t i;
    for (i = 0; i < note->voice->size; ++i) {
        if (i <= note->octxs->size) {
            fz_list_append(note->octxs, &_fz_default_octx);
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
              fz_list_t *samples)
{
    fz_result_t result = _fz_note_sync(note);
    if (result != FZ_RESULT_SUCCESS) {
        return result;
    }
    return FZ_RESULT_NOT_IMPLEMENTED;
}