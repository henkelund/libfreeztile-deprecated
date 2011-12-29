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
#include <math.h>
/*#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_real_float.h>
#include <gsl/gsl_fft_halfcomplex_float.h>*/
#include "filter.h"
#include "freeztile.h"
#include "list.h"
#include "form.h"

fz_result_t
fz_filter_create(fz_filter_t **filter)
{
    fz_filter_t *f = malloc(sizeof(fz_filter_t));
    if (f == NULL) {
        return FZ_RESULT_MALLOC_ERROR;
    }
    f->function = NULL;
    f->options  = FZ_FILTER_OPT_NONE;
    fz_form_create(&f->envelope.adsr[FZ_ENV_ATTACK]);
    fz_form_create(&f->envelope.adsr[FZ_ENV_DECAY]);
    fz_form_create(&f->envelope.adsr[FZ_ENV_SUSTAIN]);
    fz_form_create(&f->envelope.adsr[FZ_ENV_RELEASE]);
    f->envelope.sizes[FZ_ENV_ATTACK]  = 0;
    f->envelope.sizes[FZ_ENV_DECAY]   = 0;
    f->envelope.sizes[FZ_ENV_SUSTAIN] = 0;
    f->envelope.sizes[FZ_ENV_RELEASE] = 0;
    f->envelope.state = FZ_ENV_ATTACK;
    FZ_LIST_NEW(f->env_buffer, fz_real_t);
    f->envelope.blend  = 0;
    f->data     = NULL;
    *filter     = f;
    return FZ_RESULT_SUCCESS;
}

fz_uint_t
fz_filter_apply(
                fz_filter_t *filter, 
                fz_list_t   *samples)
{
    fz_uint_t i;
    if (filter->function != NULL) {
        // @todo if opt use env -> progress env
        fz_list_clear(filter->env_buffer, samples->size);
        for (i = 0; i < filter->env_buffer->size; ++i) {
            FZ_LIST_VAL(filter->env_buffer, i, fz_real_t) = 1;
        }
        return filter->function(filter, samples, filter->env_buffer);
    }
    return 0;
}

fz_result_t
fz_filter_lowpass_create(fz_filter_t **filter)
{
    fz_result_t result;
    fz_filter_lowpass_t *f;
    if ((result = fz_filter_create(filter)) != FZ_RESULT_SUCCESS) {
        return result;
    }
    f = malloc(sizeof(fz_filter_lowpass_t));
    if (f == NULL) {
        // @todo fz_filter_destroy(filter);
        return FZ_RESULT_MALLOC_ERROR;
    }
    f->rc   = 0.f;
    f->last = 0.f;
    (*filter)->data = f;
    (*filter)->function = fz_filter_lowpass_function;
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_filter_lowpass_rc_set(
                         fz_filter_t *filter,
                         fz_float_t  cutoff)
{
    if (filter == NULL || filter->data == NULL) {
        return FZ_RESULT_INVALID_ARG;
    }
    ((fz_filter_lowpass_t*) filter->data)->rc = fabs(cutoff);
    return FZ_RESULT_SUCCESS;
}

fz_result_t
fz_filter_lowpass_rc_get(
                         fz_filter_t *filter,
                         fz_float_t  *cutoff)
{
    if (filter == NULL || filter->data == NULL) {
        return FZ_RESULT_INVALID_ARG;
    }
    *cutoff = ((fz_filter_lowpass_t*) filter->data)->rc;
    return FZ_RESULT_SUCCESS;
}

fz_uint_t
fz_filter_lowpass_function(
                           fz_filter_t *filter,
                           fz_list_t   *samples,
                           fz_list_t   *envelope)
{
    fz_filter_lowpass_t *lowpass;
    fz_uint_t i;
    fz_float_t dt = samples->size, a;
    if (samples->size == 0 && filter->data != NULL) {
        return 0;
    }
    lowpass = (fz_filter_lowpass_t*) filter->data;
    dt = (fz_float_t)samples->size;
    a = dt/(lowpass->rc*FZ_LIST_VAL(envelope, 0, fz_real_t) + dt);
    // @todo use envelope to recalc a in loop

    // calc first amplitude from stored history
    FZ_LIST_VAL(samples, 0, fz_amp_t) =
        lowpass->last + (a*(FZ_LIST_VAL(samples, 0, fz_amp_t) - lowpass->last));

    for (i = 1; i < samples->size; ++i) {
        a = dt/(lowpass->rc*FZ_LIST_VAL(envelope, i, fz_real_t) + dt);
        FZ_LIST_VAL(samples, i, fz_amp_t) =
            FZ_LIST_VAL(samples, i - 1, fz_amp_t) +
                (
                    a*
                        (
                            FZ_LIST_VAL(samples, i, fz_amp_t) -
                            FZ_LIST_VAL(samples, i - 1, fz_amp_t)
                         )
                );
        // i.e. samples[i] = samples[i-1] + α * (samples[i] - samples[i-1]);
    }
    lowpass->last = FZ_LIST_VAL(samples, samples->size - 1, fz_amp_t);
    return samples->size;
}

/*#include <stdio.h>

fz_uint_t
fz_filter_equalizer_apply(
                          fz_filter_t *filter,
                          fz_list_t   *samples)
{
    (void)filter;
    fz_uint_t i;
    fz_float_t data[samples->size];
    gsl_fft_real_wavetable_float *real;
    gsl_fft_halfcomplex_wavetable_float *hc;
    gsl_fft_real_workspace_float *work;

    for (i = 0; i < samples->size; ++i) {
        data[i] = FZ_LIST_REF(samples, i, fz_sample_t)->value;
        printf("%d;%d\n", i, (short) (((float)SHRT_MAX)*data[i]));
    }

    // @todo reuse allocated wavetables & workspace if big enough
    work = gsl_fft_real_workspace_float_alloc(samples->size);

    real = gsl_fft_real_wavetable_float_alloc(samples->size);
    gsl_fft_real_float_transform(data, 1, samples->size, real, work);
    gsl_fft_real_wavetable_float_free(real);

    float rel = 0;
    for (i = 0; i < samples->size; ++i) {
        if (data[i] > rel) rel = data[i];
    }
    rel = 1.f/rel;

    for (i = 0; i < samples->size; ++i) {
        if (i > 10) data[i] *= 0;
        printf("%d;%d\n", i, (short) (((float)SHRT_MAX)*data[i]*rel));
    }

    // t = bufferstorlek i sekunder
    // freq = (trans x)/t

    hc = gsl_fft_halfcomplex_wavetable_float_alloc(samples->size);
    gsl_fft_halfcomplex_float_inverse(data, 1, samples->size, hc, work);
    gsl_fft_halfcomplex_wavetable_float_free(hc);

    gsl_fft_real_workspace_float_free(work);

    for (i = 0; i < samples->size; ++i) {
        FZ_LIST_REF(samples, i, fz_sample_t)->value = data[i];
        printf("%d;%d\n", i, (short) (((float)SHRT_MAX)*data[i]));
    }

    return samples->size;
}*/
