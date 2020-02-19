/* -*- c++ -*- */
/*
 * Copyright 2005,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "uchar_array_to_float.h"

void uchar_array_to_float(const unsigned char* in, float* out, int nsamples)
{
    while (nsamples >= 4) {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        out[3] = in[3];
        out += 4;
        in += 4;
        nsamples -= 4;
    }

    while (nsamples-- > 0)
        *out++ = *in++;
}
