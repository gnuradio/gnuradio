/* -*- c++ -*- */
/*
 * Copyright 2002,2012 Free Software Foundation, Inc.
 * Copyrigth 2025 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/filter/interpolator_taps.h>
#include <gnuradio/filter/mmse_fir_interpolator.h>
#include <gnuradio/gr_complex.h>
#include <stdexcept>

namespace gr {
namespace filter {

namespace {
template <typename sample_t>
std::vector<kernel::fir_filter<sample_t, sample_t, float>> build_filters()
{
    std::vector<kernel::fir_filter<sample_t, sample_t, float>> filters;
    filters.reserve(NSTEPS + 1);
    for (int i = 0; i < NSTEPS + 1; i++) {
        std::vector<float> t(&taps[i][0], &taps[i][NTAPS]);
        filters.emplace_back(std::move(t));
    }
    return filters;
}
} // namespace

template <typename sample_t>
mmse_fir_interpolator<sample_t>::mmse_fir_interpolator()
    : filters(build_filters<sample_t>())
{
}

template <typename sample_t>
unsigned mmse_fir_interpolator<sample_t>::ntaps() const
{
    return NTAPS;
}

template <typename sample_t>
unsigned mmse_fir_interpolator<sample_t>::nsteps() const
{
    return NSTEPS;
}

template <typename sample_t>
sample_t mmse_fir_interpolator<sample_t>::interpolate(const sample_t input[],
                                                      float mu) const
{
    int imu = (int)rint(mu * NSTEPS);

    if ((imu < 0) || (imu > NSTEPS)) {
        throw std::runtime_error("mmse_fir_interpolator<sample_t>: imu out of bounds.");
    }

    return filters[imu].filter(input);
}

template class mmse_fir_interpolator<float>;
template class mmse_fir_interpolator<gr_complex>;

} /* namespace filter */
} /* namespace gr */
