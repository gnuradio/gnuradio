/* -*- c++ -*- */
/*
 * Copyright 2002,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "interleaved_short_array_to_complex.h"
#include <assert.h>
#include <volk/volk.h>

void interleaved_short_array_to_complex(const short* in, gr_complex* out, int nsamples)
{
    assert(nsamples % 2 == 0);
    volk_16i_s32f_convert_32f_u((float*)out, (const int16_t*)in, 1, nsamples);
}
