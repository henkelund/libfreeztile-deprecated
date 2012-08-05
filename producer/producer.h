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

#ifndef _FZ_PRODUCER_H_
#define _FZ_PRODUCER_H_

#include "../freeztile.h"

BEGIN_C_DECLS

typedef struct fz_producer_s {
    const fz_ptr_t   _class;
    fz_int_t       (*produce)(struct fz_producer_s *self, fz_list_t *buffer);
} fz_producer_t;

const fz_ptr_t fz_producer;

/**
 *
 * @param  fz_ptr_t             producer
 * @param  fz_list_t<fz_amp_t> *buffer
 * @return The number of produced samples or a negative error code
 */
fz_result_t fz_produce PARAMS((
                               fz_ptr_t   producer,
                               fz_list_t *buffer));

END_C_DECLS

#endif // _FZ_PRODUCER_H_
