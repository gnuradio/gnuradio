/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_PFB_CLOCK_SYNC_CCF_IMPL_H
#define INCLUDED_DIGITAL_PFB_CLOCK_SYNC_CCF_IMPL_H

#include <gnuradio/digital/pfb_clock_sync_ccf.h>


namespace gr {
namespace digital {

class pfb_clock_sync_ccf_impl : public pfb_clock_sync_ccf
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
    std::vector<filter::kernel::fir_filter_ccf> d_filters;
    std::vector<filter::kernel::fir_filter_ccf> d_diff_filters;
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

    uint64_t d_old_in, d_new_in, d_last_out;

    void create_diff_taps(const std::vector<float>& newtaps,
                          std::vector<float>& difftaps);

    void set_taps(const std::vector<float>& taps,
                  std::vector<std::vector<float>>& ourtaps,
                  std::vector<filter::kernel::fir_filter_ccf>& ourfilter);

public:
    pfb_clock_sync_ccf_impl(double sps,
                            float loop_bw,
                            const std::vector<float>& taps,
                            unsigned int filter_size = 32,
                            float init_phase = 0,
                            float max_rate_deviation = 1.5,
                            int osps = 1);

    void setup_rpc() override;

    void update_gains() override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    void update_taps(const std::vector<float>& taps) override;

    std::vector<std::vector<float>> taps() const override;
    std::vector<std::vector<float>> diff_taps() const override;
    std::vector<float> channel_taps(int channel) const override;
    std::vector<float> diff_channel_taps(int channel) const override;
    std::string taps_as_string() const override;
    std::string diff_taps_as_string() const override;

    void set_loop_bandwidth(float bw) override;
    void set_damping_factor(float df) override;
    void set_alpha(float alpha) override;
    void set_beta(float beta) override;
    void set_max_rate_deviation(float m) override { d_max_dev = m; }

    float loop_bandwidth() const override;
    float damping_factor() const override;
    float alpha() const override;
    float beta() const override;
    float clock_rate() const override;

    float error() const override;
    float rate() const override;
    float phase() const override;

    /*******************************************************************
     *******************************************************************/

    bool check_topology(int ninputs, int noutputs) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_PFB_CLOCK_SYNC_CCF_IMPL_H */
