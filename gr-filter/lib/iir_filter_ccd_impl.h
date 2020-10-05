/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIR_FILTER_CCD_IMPL_H
#define INCLUDED_IIR_FILTER_CCD_IMPL_H

#include <gnuradio/filter/iir_filter.h>
#include <gnuradio/filter/iir_filter_ccd.h>

namespace gr {
namespace filter {

class FILTER_API iir_filter_ccd_impl : public iir_filter_ccd
{
private:
    bool d_updated;
    kernel::iir_filter<gr_complex, gr_complex, double, gr_complexd> d_iir;
    std::vector<double> d_new_fftaps;
    std::vector<double> d_new_fbtaps;

public:
    iir_filter_ccd_impl(const std::vector<double>& fftaps,
                        const std::vector<double>& fbtaps,
                        bool oldstyle = true);

    void set_taps(const std::vector<double>& fftaps,
                  const std::vector<double>& fbtaps) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_IIR_FILTER_CCD_IMPL_H */
