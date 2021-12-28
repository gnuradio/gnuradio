/* -*- c++ -*- */
/*
 * Copyright (C) 2002,2012,2017 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gnuradio/filter/interp_differentiator_taps.h"
#include <gnuradio/filter/mmse_interp_differentiator_ff.h>
#include <stdexcept>

namespace gr {
namespace filter {

namespace {
std::vector<kernel::fir_filter_fff> build_filters()
{
    std::vector<kernel::fir_filter_fff> filters;
    filters.reserve(DNSTEPS + 1);
    for (int i = 0; i < DNSTEPS + 1; i++) {
        std::vector<float> t(&Dtaps[i][0], &Dtaps[i][DNTAPS]);
        filters.emplace_back(std::move(t));
    }
    return filters;
}
} // namespace

mmse_interp_differentiator_ff::mmse_interp_differentiator_ff() : filters(build_filters())
{
}

unsigned mmse_interp_differentiator_ff::ntaps() const { return DNTAPS; }

unsigned mmse_interp_differentiator_ff::nsteps() const { return DNSTEPS; }

float mmse_interp_differentiator_ff::differentiate(const float input[], float mu) const
{
    int imu = (int)rint(mu * DNSTEPS);

    if ((imu < 0) || (imu > DNSTEPS)) {
        throw std::runtime_error("mmse_interp_differentiator_ff: imu out of bounds.");
    }

    float r = filters[imu].filter(input);
    return r;
}

} /* namespace filter */
} /* namespace gr */
