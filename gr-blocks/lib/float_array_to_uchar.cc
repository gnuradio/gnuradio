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
#include <config.h>
#endif

#define _ISOC9X_SOURCE
#include "float_array_to_uchar.h"
#include <cmath>

static const int MIN_UCHAR = 0;
static const int MAX_UCHAR = 255;

void float_array_to_uchar(const float* in, unsigned char* out, int nsamples)
{
    for (int i = 0; i < nsamples; i++) {
        long int r = (long int)rint(in[i]);
        if (r < MIN_UCHAR)
            r = MIN_UCHAR;
        else if (r > MAX_UCHAR)
            r = MAX_UCHAR;
        out[i] = r;
    }
}
