/* -*- c++ -*- */
/*
 * Copyright 2011-2012 Free Software Foundation, Inc.
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
#include "float_array_to_int.h"
#include <cmath>
#if __cplusplus >= 201103L
#include <cstdint>
using std::int64_t;
#else
#include <stdint.h>
#endif

static const int64_t MAX_INT = INT32_MAX;
static const int64_t MIN_INT = INT32_MIN;

void float_array_to_int(const float* in, int* out, float scale, int nsamples)
{
    for (int i = 0; i < nsamples; i++) {
        int64_t r = llrintf(scale * in[i]);
        if (r < MIN_INT)
            r = MIN_INT;
        else if (r > MAX_INT)
            r = MAX_INT;
        out[i] = static_cast<int>(r);
    }
}
