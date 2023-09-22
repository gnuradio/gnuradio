/* -*- c++ -*- */
/*
 * Copyright 2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_PFB_SYNTHESIZER_CCF_IMPL_H
#define INCLUDED_PFB_SYNTHESIZER_CCF_IMPL_H

#include <gnuradio/fft/fft.h>
#include <gnuradio/filter/fir_filter_with_buffer.h>
#include <gnuradio/filter/pfb_synthesizer_ccf.h>
#include <gnuradio/thread/thread.h>

namespace gr {
namespace filter {

// While this is a polyphase_filterbank, we don't use the normal
// parent class because we have to use the fir_filter_with_buffer
// objects instead of normal filters.

class FILTER_API pfb_synthesizer_ccf_impl : public pfb_synthesizer_ccf
{
private:
    bool d_updated;
    unsigned int d_numchans;
    unsigned int d_taps_per_filter;
    fft::fft_complex_rev* d_fft;
    std::vector<kernel::fir_filter_with_buffer_ccf> d_filters;
    std::vector<std::vector<float>> d_taps;
    int d_state;
    std::vector<int> d_channel_map;
    unsigned int d_twox;
    gr::thread::mutex d_mutex; // mutex to protect set/work access

    /*!
     * \brief Tap setting algorithm for critically sampled channels
     */
    void set_taps1(const std::vector<float>& taps);

    /*!
     * \brief Tap setting algorithm for 2x over-sampled channels
     */
    void set_taps2(const std::vector<float>& taps);


public:
    pfb_synthesizer_ccf_impl(unsigned int numchans,
                             const std::vector<float>& taps,
                             bool twox);
    void set_taps(const std::vector<float>& taps) override;
    std::vector<std::vector<float>> taps() const override;
    void print_taps() override;

    void set_channel_map(const std::vector<int>& map) override;
    std::vector<int> channel_map() const override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_PFB_SYNTHESIZER_CCF_IMPL_H */
