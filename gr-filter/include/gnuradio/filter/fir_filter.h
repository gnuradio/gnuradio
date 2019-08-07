/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
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
