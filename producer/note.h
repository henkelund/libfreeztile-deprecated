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

#ifndef _FZ_NOTE_H_
#define _FZ_NOTE_H_

#include "../freeztile.h"
#include "../util/map.h"
#include "../filter/amplifier.h"

BEGIN_C_DECLS

#define FZ_AMPLIFIER_KEY "amplifier"

#define FZ_NOTE_FLAG_NONE    0
#define FZ_NOTE_FLAG_ACTIVE (1 << 0)

typedef fz_list_t/*<fz_oscdesc_t*>*/ fz_voice_t;

typedef struct {
    fz_producer_t  _super;
    fz_voice_t    *voice;
    fz_list_t     *oscillators;
    fz_real_t      freq;
    fz_uint_t      sample_rate;
    fz_flags_t     flags;
    fz_list_t     *ob;
    fz_map_t      *envelopes;
    fz_list_t     *env_ob;
    fz_map_t      *filters;
} fz_note_t;

/**
 * Parse a frequency from a given string
 *
 * @param  const fz_char_t* name
 * @return fz_real_t
 */
fz_real_t   fz_note_parse_frequency PARAMS((const fz_char_t *name));

/**
 *
 * @param  fz_note_t* note
 * @return fz_bool_t
 */
fz_bool_t   fz_note_is_active PARAMS((fz_note_t *note));

/**
 *
 * @param  fz_note_t   *note
 * @param  fz_amp_t     amplitude
 * @return fz_result_t
 */
fz_result_t fz_note_start PARAMS((
                                  fz_note_t *note,
                                  fz_amp_t   amplitude));

/**
 *
 * @param  fz_note_t   *note
 * @return fz_result_t
 */
fz_result_t fz_note_stop PARAMS((fz_note_t *note));

const fz_ptr_t fz_note;

END_C_DECLS

#endif // _FZ_NOTE_H_
