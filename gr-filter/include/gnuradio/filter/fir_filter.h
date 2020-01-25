/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FILTER_FIR_FILTER_H
#define INCLUDED_FILTER_FIR_FILTER_H

#include <gnuradio/filter/api.h>
#include <gnuradio/gr_complex.h>
#include <cstdint>
#include <vector>

namespace gr {
namespace filter {
namespace kernel {

template <class IN_T, class OUT_T, class TAP_T>
class FILTER_API fir_filter
{
public:
    fir_filter(int decimation, const std::vector<TAP_T>& taps);
    ~fir_filter();

    void set_taps(const std::vector<TAP_T>& taps);
    void update_tap(TAP_T t, unsigned int index);
    std::vector<TAP_T> taps() const;
    unsigned int ntaps() const;

    OUT_T filter(const IN_T input[]);
    void filterN(OUT_T output[], const IN_T input[], unsigned long n);
    void filterNdec(OUT_T output[],
                    const IN_T input[],
                    unsigned long n,
                    unsigned int decimate);

protected:
    std::vector<TAP_T> d_taps;
    unsigned int d_ntaps;
    TAP_T** d_aligned_taps;
    OUT_T* d_output;
    int d_align;
    int d_naligned;
};
typedef fir_filter<float, float, float> fir_filter_fff;
typedef fir_filter<gr_complex, gr_complex, float> fir_filter_ccf;
typedef fir_filter<float, gr_complex, gr_complex> fir_filter_fcc;
typedef fir_filter<gr_complex, gr_complex, gr_complex> fir_filter_ccc;
typedef fir_filter<std::int16_t, gr_complex, gr_complex> fir_filter_scc;
typedef fir_filter<float, std::int16_t, float> fir_filter_fsf;
} /* namespace kernel */
} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_FIR_FILTER_H */
