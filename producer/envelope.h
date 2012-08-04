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

#ifndef _FZ_ENVELOPE_H_
#define _FZ_ENVELOPE_H_

#include "producer.h"
#include "../freeztile.h"

BEGIN_C_DECLS

#define FZ_ENV_ATTACK   0
#define FZ_ENV_DECAY    1
#define FZ_ENV_SUSTAIN  2
#define FZ_ENV_RELEASE  3
#define FZ_ENV_SILENT  -1

typedef struct {
    fz_object_t  *_class;
    fz_curve_t    curves    [4];
    fz_uint_t     durations [4];
    fz_form_t    *cache     [4];
} fz_envdesc_t;

typedef struct {
    fz_producer_t  _super;
    fz_envdesc_t  *descriptor;
    fz_int_t       state;
    fz_uint_t      frame;
    fz_real_t      factor;
} fz_envelope_t;

const fz_ptr_t fz_envdesc;
const fz_ptr_t fz_envelope;

/**
 *
 * @param fz_envelope_t *envelope
 */
void fz_envelope_attack PARAMS((fz_envelope_t *envelope));

/**
 *
 * @param fz_envelope_t *envelope
 */
void fz_envelope_release PARAMS((fz_envelope_t *envelope));

END_C_DECLS

#endif // _FZ_ENVELOPE_H_
