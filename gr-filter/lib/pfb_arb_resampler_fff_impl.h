/* -*- c++ -*- */
/*
 * Copyright 2009-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_PFB_ARB_RESAMPLER_FFF_IMPL_H
#define INCLUDED_PFB_ARB_RESAMPLER_FFF_IMPL_H

#include <gnuradio/filter/pfb_arb_resampler.h>
#include <gnuradio/filter/pfb_arb_resampler_fff.h>
#include <gnuradio/thread/thread.h>

namespace gr {
namespace filter {

class FILTER_API pfb_arb_resampler_fff_impl : public pfb_arb_resampler_fff
{
private:
    kernel::pfb_arb_resampler_fff* d_resamp;
    bool d_updated;
    gr::thread::mutex d_mutex; // mutex to protect set/work access

public:
    pfb_arb_resampler_fff_impl(float rate,
                               const std::vector<float>& taps,
                               unsigned int filter_size);

    ~pfb_arb_resampler_fff_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    void set_taps(const std::vector<float>& taps);
    std::vector<std::vector<float>> taps() const;
    void print_taps();

    void set_rate(float rate);
    void set_phase(float ph);
    float phase() const;

    unsigned int interpolation_rate() const;
    unsigned int decimation_rate() const;
    float fractional_rate() const;
    unsigned int taps_per_filter() const;

    int group_delay() const;
    float phase_offset(float freq, float fs);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_PFB_ARB_RESAMPLER_FFF_IMPL_H */
