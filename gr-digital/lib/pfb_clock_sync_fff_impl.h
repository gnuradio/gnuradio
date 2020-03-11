/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_PFB_CLOCK_SYNC_FFF_IMPL_H
#define INCLUDED_DIGITAL_PFB_CLOCK_SYNC_FFF_IMPL_H

#include <gnuradio/digital/pfb_clock_sync_fff.h>

using namespace gr::filter;

namespace gr {
namespace digital {

class pfb_clock_sync_fff_impl : public pfb_clock_sync_fff
{
private:
    bool d_updated;
    double d_sps;
    float d_loop_bw;
    float d_damping;
    float d_alpha;
    float d_beta;

    const int d_nfilters;
    int d_taps_per_filter;
    std::vector<kernel::fir_filter_fff*> d_filters;
    std::vector<kernel::fir_filter_fff*> d_diff_filters;
    std::vector<std::vector<float>> d_taps;
    std::vector<std::vector<float>> d_dtaps;
    std::vector<float> d_updated_taps;

    float d_k;
    float d_rate;
    float d_rate_i;
    float d_rate_f;
    float d_max_dev;
    int d_filtnum;
    int d_osps;
    float d_error;
    int d_out_idx;

    void create_diff_taps(const std::vector<float>& newtaps,
                          std::vector<float>& difftaps);

    void set_taps(const std::vector<float>& taps,
                  std::vector<std::vector<float>>& ourtaps,
                  std::vector<kernel::fir_filter_fff*>& ourfilter);

public:
    pfb_clock_sync_fff_impl(double sps,
                            float gain,
                            const std::vector<float>& taps,
                            unsigned int filter_size = 32,
                            float init_phase = 0,
                            float max_rate_deviation = 1.5,
                            int osps = 1);
    ~pfb_clock_sync_fff_impl();

    void update_gains();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    void update_taps(const std::vector<float>& taps);

    std::vector<std::vector<float>> taps() const;
    std::vector<std::vector<float>> diff_taps() const;
    std::vector<float> channel_taps(int channel) const;
    std::vector<float> diff_channel_taps(int channel) const;
    std::string taps_as_string() const;
    std::string diff_taps_as_string() const;

    void set_loop_bandwidth(float bw);
    void set_damping_factor(float df);
    void set_alpha(float alpha);
    void set_beta(float beta);
    void set_max_rate_deviation(float m) { d_max_dev = m; }

    float loop_bandwidth() const;
    float damping_factor() const;
    float alpha() const;
    float beta() const;
    float clock_rate() const;

    bool check_topology(int ninputs, int noutputs);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_PFB_CLOCK_SYNC_FFF_IMPL_H */
