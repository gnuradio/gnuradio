/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_FILTER_SINGLE_POLE_IIR_QUICK_START_H
#define INCLUDED_GR_FILTER_SINGLE_POLE_IIR_QUICK_START_H

#include <gnuradio/filter/single_pole_iir.h>
#include <cstdint>

namespace gr {
namespace filter {

template <class T>
class single_pole_iir_quick_start : private single_pole_iir<T, T, double>
{
    using base = single_pole_iir<T, T, double>;

    // Zero disables the running mean after reaching the configured alpha.
    std::uint64_t d_next_sample;

public:
    single_pole_iir_quick_start(double alpha, bool quick_start);

    using base::set_taps;
    T filter(T input);
};

} // namespace filter
} // namespace gr

#endif /* INCLUDED_GR_FILTER_SINGLE_POLE_IIR_QUICK_START_H */
