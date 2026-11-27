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
    uint64_t d_rate_in = 0;  // nitems_read(0) at the first work() after set_rate()
    uint64_t d_rate_out = 0; // nitems_written(0) at the first work() after set_rate()
    uint64_t d_tag_next = 0; // first input offset whose tags are not placed yet
    bool d_rate_changed = false;
    gr::thread::mutex d_mutex; // mutex to protect set/work access

public:
    pfb_arb_resampler_ccf_impl(float rate,
                               const std::vector<float>& taps,
                               unsigned int filter_size);

    bool start() override
    {
        // item counters start from 0 again when the flowgraph is restarted
        d_rate_in = d_rate_out = d_tag_next = 0;
        d_rate_changed = false;
        return true;
    }

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
