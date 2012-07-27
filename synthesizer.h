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
#include "util/list.h"

typedef struct {
    const fz_ptr_t             _scp;
    fz_list_t/*<fz_amp_t>*/   *ob;
    fz_list_t/*<fz_note_t*>*/ *note_pool;
} fz_synthesizer_t;

fz_list_t/*<fz_amp_t>*/*
fz_synthesizer_output(
                      fz_synthesizer_t *synth,
                      fz_uint_t         num_frames);

const fz_ptr_t fz_synthesizer;

#endif // _FZ_SYNTHESIZER_H_
