/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/gr_complex.h>
#include <volk/volk_alloc.hh>
#include <cstdint>
#include <vector>

namespace gr {
namespace kernel {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
class fir_filter
{
public:
    fir_filter(const std::vector<TAP_T>& taps);

    // Disallow copy.
    //
    // This prevents accidentally doing needless copies, not just of fir_filter,
    // but every block that contains one.
    fir_filter(const fir_filter&) = delete;
    fir_filter& operator=(const fir_filter&) = delete;
    fir_filter(fir_filter&&) = default;
    fir_filter& operator=(fir_filter&&) = default;

    void set_taps(const std::vector<TAP_T>& taps);
    void update_tap(TAP_T t, unsigned int index);
    std::vector<TAP_T> taps() const;
    unsigned int ntaps() const;

    OUT_T filter(const IN_T input[]) const;
    void filterN(OUT_T output[], const IN_T input[], unsigned long n);
    void filterNdec(OUT_T output[],
                    const IN_T input[],
                    unsigned long n,
                    unsigned int decimate);

protected:
    std::vector<TAP_T> d_taps;
    unsigned int d_ntaps;
    std::vector<volk::vector<TAP_T>> d_aligned_taps;
    volk::vector<OUT_T> d_output;
    int d_align;
    int d_naligned;
};
using fir_filter_fff = fir_filter<float, float, float>;
using fir_filter_ccf = fir_filter<gr_complex, gr_complex, float>;
using fir_filter_fcc = fir_filter<float, gr_complex, gr_complex>;
using fir_filter_ccc = fir_filter<gr_complex, gr_complex, gr_complex>;
using fir_filter_scc = fir_filter<std::int16_t, gr_complex, gr_complex>;
using fir_filter_fsf = fir_filter<float, std::int16_t, float>;
} // namespace filter
} // namespace kernel
} /* namespace gr */
