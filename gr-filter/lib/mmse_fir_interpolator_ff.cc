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

#include <gnuradio/filter/interpolator_taps.h>
#include <gnuradio/filter/mmse_fir_interpolator_ff.h>
#include <stdexcept>

namespace gr {
namespace filter {

namespace {
std::vector<kernel::fir_filter_fff> build_filters()
{
    std::vector<kernel::fir_filter_fff> filters;
    filters.reserve(NSTEPS + 1);
    for (int i = 0; i < NSTEPS + 1; i++) {
        std::vector<float> t(&taps[i][0], &taps[i][NTAPS]);
        filters.emplace_back(t);
    }
    return filters;
}
} // namespace

mmse_fir_interpolator_ff::mmse_fir_interpolator_ff() : filters(build_filters()) {}

unsigned mmse_fir_interpolator_ff::ntaps() const { return NTAPS; }

unsigned mmse_fir_interpolator_ff::nsteps() const { return NSTEPS; }

float mmse_fir_interpolator_ff::interpolate(const float input[], float mu) const
{
    int imu = (int)rint(mu * NSTEPS);

    if ((imu < 0) || (imu > NSTEPS)) {
        throw std::runtime_error("mmse_fir_interpolator_ff: imu out of bounds.");
    }

    float r = filters[imu].filter(input);
    return r;
}

} /* namespace filter */
} /* namespace gr */
