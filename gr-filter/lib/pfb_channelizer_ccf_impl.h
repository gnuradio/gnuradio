/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FILTER_PFB_CHANNELIZER_CCF_IMPL_H
#define INCLUDED_FILTER_PFB_CHANNELIZER_CCF_IMPL_H

#include <gnuradio/fft/fft.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/filter/pfb_channelizer_ccf.h>
#include <gnuradio/filter/polyphase_filterbank.h>
#include <gnuradio/thread/thread.h>

namespace gr {
namespace filter {

class FILTER_API pfb_channelizer_ccf_impl : public pfb_channelizer_ccf,
                                            kernel::polyphase_filterbank
{
private:
    bool d_updated;
    float d_oversample_rate;
    std::vector<int> d_idxlut;
    int d_rate_ratio;
    int d_output_multiple;
    std::vector<int> d_channel_map;
    gr::thread::mutex d_mutex; // mutex to protect set/work access

public:
    pfb_channelizer_ccf_impl(unsigned int nfilts,
                             const std::vector<float>& taps,
                             float oversample_rate);

    void set_taps(const std::vector<float>& taps) override;
    void print_taps() override;
    std::vector<std::vector<float>> taps() const override;

    void set_channel_map(const std::vector<int>& map) override;
    std::vector<int> channel_map() const override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace filter */
} /* namespace gr */

#endif
