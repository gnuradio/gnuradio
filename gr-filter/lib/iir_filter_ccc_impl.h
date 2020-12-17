/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIR_FILTER_CCC_IMPL_H
#define INCLUDED_IIR_FILTER_CCC_IMPL_H

#include <gnuradio/filter/iir_filter.h>
#include <gnuradio/filter/iir_filter_ccc.h>

namespace gr {
namespace filter {

class FILTER_API iir_filter_ccc_impl : public iir_filter_ccc
{
private:
    bool d_updated;
    kernel::iir_filter<gr_complex, gr_complex, gr_complex, gr_complex> d_iir;
    std::vector<gr_complex> d_new_fftaps;
    std::vector<gr_complex> d_new_fbtaps;

public:
    iir_filter_ccc_impl(const std::vector<gr_complex>& fftaps,
                        const std::vector<gr_complex>& fbtaps,
                        bool oldstyle = true);

    void set_taps(const std::vector<gr_complex>& fftaps,
                  const std::vector<gr_complex>& fbtaps) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_IIR_FILTER_CCC_IMPL_H */
