/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/filter/pfb_channelizer.h>
#include <gnuradio/kernel/filter/polyphase_filterbank.h>

#include <mutex>

namespace gr {
namespace filter {

template <class T>
class pfb_channelizer_cpu : public pfb_channelizer<T>,
                            kernel::filter::polyphase_filterbank
{
public:
    pfb_channelizer_cpu(const typename pfb_channelizer<T>::block_args& args);

    work_return_code_t work(work_io&) override;

    int group_delay();
    void set_taps(const std::vector<float>& taps) override;

private:
    bool d_updated = false;
    float d_oversample_rate;
    std::vector<int> d_idxlut;
    int d_rate_ratio;
    int d_output_multiple;
    std::vector<int> d_channel_map;
    std::mutex d_mutex; // mutex to protect set/work access

    size_t d_history = 1;

    size_t d_nchans;

    std::vector<std::vector<T>> d_deinterleaved;
};


} // namespace filter
} // namespace gr
