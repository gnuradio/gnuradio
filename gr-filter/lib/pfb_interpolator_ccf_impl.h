/* -*- c++ -*- */
/*
 * Copyright 2009,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_PFB_INTERPOLATOR_CCF_IMPL_H
#define INCLUDED_PFB_INTERPOLATOR_CCF_IMPL_H

#include <gnuradio/fft/fft.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/filter/pfb_interpolator_ccf.h>
#include <gnuradio/filter/polyphase_filterbank.h>
#include <gnuradio/thread/thread.h>

namespace gr {
namespace filter {

class FILTER_API pfb_interpolator_ccf_impl : public pfb_interpolator_ccf,
                                             kernel::polyphase_filterbank
{
private:
    bool d_updated;
    unsigned int d_rate;
    gr::thread::mutex d_mutex; // mutex to protect set/work access

public:
    pfb_interpolator_ccf_impl(unsigned int interp, const std::vector<float>& taps);

    void set_taps(const std::vector<float>& taps) override;
    void print_taps() override;
    std::vector<std::vector<float>> taps() const override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_PFB_INTERPOLATOR_CCF_IMPL_H */
