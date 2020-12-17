/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_PFB_ARB_RESAMPLER_CCF_IMPL_H
#define INCLUDED_PFB_ARB_RESAMPLER_CCF_IMPL_H

#include <gnuradio/filter/pfb_arb_resampler.h>
#include <gnuradio/filter/pfb_arb_resampler_ccf.h>
#include <gnuradio/thread/thread.h>

namespace gr {
namespace filter {

class FILTER_API pfb_arb_resampler_ccf_impl : public pfb_arb_resampler_ccf
{
private:
    kernel::pfb_arb_resampler_ccf d_resamp;
    bool d_updated;
    gr::thread::mutex d_mutex; // mutex to protect set/work access

public:
    pfb_arb_resampler_ccf_impl(float rate,
                               const std::vector<float>& taps,
                               unsigned int filter_size);

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    void set_taps(const std::vector<float>& taps) override;
    std::vector<std::vector<float>> taps() const override;
    void print_taps() override;

    void set_rate(float rate) override;
    void set_phase(float ph) override;
    float phase() const override;

    unsigned int interpolation_rate() const override;
    unsigned int decimation_rate() const override;
    float fractional_rate() const override;
    unsigned int taps_per_filter() const override;

    int group_delay() const override;
    float phase_offset(float freq, float fs) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_PFB_ARB_RESAMPLER_CCF_IMPL_H */
