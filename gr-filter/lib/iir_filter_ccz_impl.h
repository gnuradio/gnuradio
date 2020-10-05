/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIR_FILTER_CCZ_IMPL_H
#define INCLUDED_IIR_FILTER_CCZ_IMPL_H

#include <gnuradio/filter/iir_filter.h>
#include <gnuradio/filter/iir_filter_ccz.h>

namespace gr {
namespace filter {

class FILTER_API iir_filter_ccz_impl : public iir_filter_ccz
{
private:
    bool d_updated;
    kernel::iir_filter<gr_complex, gr_complex, gr_complexd, gr_complexd> d_iir;
    std::vector<gr_complexd> d_new_fftaps;
    std::vector<gr_complexd> d_new_fbtaps;

public:
    iir_filter_ccz_impl(const std::vector<gr_complexd>& fftaps,
                        const std::vector<gr_complexd>& fbtaps,
                        bool oldstyle = true);

    void set_taps(const std::vector<gr_complexd>& fftaps,
                  const std::vector<gr_complexd>& fbtaps) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_IIR_FILTER_CCZ_IMPL_H */
