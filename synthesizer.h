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

#ifndef _FZ_SYNTHESIZER_H_
#define _FZ_SYNTHESIZER_H_

#include "types.h"
#include "note.h"
#include "util/list.h"

#ifndef FZ_SAMPLE_RATE
#define FZ_SAMPLE_RATE 44100
#endif

#ifndef FZ_POLYPHONY_LEVEL
#define FZ_POLYPHONY_LEVEL 4
#endif

#define FZ_NOTE_STEAL_POLICY_NOSTEAL  (1 << 0)
#define FZ_NOTE_STEAL_POLICY_FIFO     (1 << 1)
#define FZ_NOTE_STEAL_POLICY_QUIETEST (1 << 2)
#define FZ_NOTE_STEAL_POLICY_EXPAND   (1 << 3)

typedef struct {
    const fz_ptr_t             _scp;
    fz_list_t/*<fz_amp_t>*/   *ob;
    fz_list_t/*<fz_note_t*>*/ *note_pool;
    fz_list_t/*<fz_note_t*>*/ *active_notes;
    fz_uint_t                  sample_rate;
    fz_voice_t                *voice;
    fz_envdesc_t              *envdesc;
    fz_flags_t                 flags;
} fz_synthesizer_t;

fz_list_t/*<fz_amp_t>*/*
fz_synthesizer_output(
                      fz_synthesizer_t *synth,
                      fz_uint_t         num_frames);

/**
 * Set the maximum level of polyphony, i.e. the sum of idle and active notes.
 *
 * @param  fz_synthesizer_t* synth
 * @param  fz_int_t          level
 * @return fz_result_t
 */
fz_result_t
fz_synthesizer_set_polyphony(fz_synthesizer_t *synth, fz_uint_t level);

/**
 * Get the maximum level of polyphony
 *
 * @param  fz_synthesizer_t* synth
 * @return fz_uint_t
 */
fz_uint_t
fz_synthesizer_get_polyphony(fz_synthesizer_t *synth);

/**
 *
 * @param  fz_synthesizer_t *synth
 * @param  fz_real_t         frequency
 * @param  fz_amp_t          amplitude
 * @return fz_note_t*
 */
fz_note_t*
fz_synthesizer_play(
                    fz_synthesizer_t *synth,
                    fz_real_t         frequency,
                    fz_amp_t          amplitude);

const fz_ptr_t fz_synthesizer;

#endif // _FZ_SYNTHESIZER_H_
