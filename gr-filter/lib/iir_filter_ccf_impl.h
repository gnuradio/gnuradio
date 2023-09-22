/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIR_FILTER_CCF_IMPL_H
#define INCLUDED_IIR_FILTER_CCF_IMPL_H

#include <gnuradio/filter/iir_filter.h>
#include <gnuradio/filter/iir_filter_ccf.h>

namespace gr {
namespace filter {

class FILTER_API iir_filter_ccf_impl : public iir_filter_ccf
{
private:
    bool d_updated;
    kernel::iir_filter<gr_complex, gr_complex, float, gr_complex> d_iir;
    std::vector<float> d_new_fftaps;
    std::vector<float> d_new_fbtaps;

public:
    iir_filter_ccf_impl(const std::vector<float>& fftaps,
                        const std::vector<float>& fbtaps,
                        bool oldstyle = true);

    void set_taps(const std::vector<float>& fftaps,
                  const std::vector<float>& fbtaps) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_IIR_FILTER_CCF_IMPL_H */
