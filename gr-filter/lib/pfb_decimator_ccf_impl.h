/* -*- c++ -*- */
/*
 * Copyright 2009,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_PFB_DECIMATOR_CCF_IMPL_H
#define INCLUDED_PFB_DECIMATOR_CCF_IMPL_H

#include <gnuradio/filter/pfb_decimator_ccf.h>
#include <gnuradio/filter/polyphase_filterbank.h>
#include <gnuradio/thread/thread.h>
#include <volk/volk_alloc.hh>

namespace gr {
namespace filter {

class FILTER_API pfb_decimator_ccf_impl : public pfb_decimator_ccf,
                                          kernel::polyphase_filterbank
{
private:
    bool d_updated;
    unsigned int d_rate;
    unsigned int d_chan;
    bool d_use_fft_rotator;
    bool d_use_fft_filters;
    std::vector<gr_complex> d_rotator;
    volk::vector<gr_complex> d_tmp; // used for fft filters
    gr::thread::mutex d_mutex;      // mutex to protect set/work access

    inline int work_fir_exp(int noutput_items,
                            gr_vector_const_void_star& input_items,
                            gr_vector_void_star& output_items);
    inline int work_fir_fft(int noutput_items,
                            gr_vector_const_void_star& input_items,
                            gr_vector_void_star& output_items);
    inline int work_fft_exp(int noutput_items,
                            gr_vector_const_void_star& input_items,
                            gr_vector_void_star& output_items);
    inline int work_fft_fft(int noutput_items,
                            gr_vector_const_void_star& input_items,
                            gr_vector_void_star& output_items);

public:
    pfb_decimator_ccf_impl(unsigned int decim,
                           const std::vector<float>& taps,
                           unsigned int channel,
                           bool use_fft_rotator = true,
                           bool use_fft_filters = true);

    // No copy because d_tmp.
    pfb_decimator_ccf_impl(const pfb_decimator_ccf_impl&) = delete;
    pfb_decimator_ccf_impl& operator=(const pfb_decimator_ccf_impl&) = delete;

    void set_taps(const std::vector<float>& taps) override;
    void print_taps() override;
    std::vector<std::vector<float>> taps() const override;
    void set_channel(const unsigned int channel) override;

    // Overload to create/destroy d_tmp based on max_noutput_items.
    bool start() override;
    bool stop() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_PFB_DECIMATOR_CCF_IMPL_H */
