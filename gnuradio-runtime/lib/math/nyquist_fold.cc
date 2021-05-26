/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#include <gnuradio/math.h>

#include <cstdint>

namespace gr {

double nyquist_fold(double f, double f_s)
{
    double f_n = f_s / 2;

    bool neg = f < 0;
    if (neg)
        f = -f;

    int64_t n = static_cast<int64_t>((f / f_n + 1) / 2);

    double r = f - n * f_s;
    if (neg)
        r = -r;

    return r;
}

} // namespace gr
