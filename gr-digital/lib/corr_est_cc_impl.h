/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CORR_EST_CC_IMPL_H
#define INCLUDED_DIGITAL_CORR_EST_CC_IMPL_H

#include <gnuradio/digital/corr_est_cc.h>
#include <gnuradio/filter/fft_filter.h>

using namespace gr::filter;

namespace gr {
namespace digital {

class corr_est_cc_impl : public corr_est_cc
{
private:
    pmt::pmt_t d_src_id;
    std::vector<gr_complex> d_symbols;
    float d_sps;
    unsigned int d_mark_delay, d_stashed_mark_delay;
    float d_thresh, d_stashed_threshold;
    kernel::fft_filter_ccc* d_filter;

    gr_complex* d_corr;
    float* d_corr_mag;

    float d_scale;
    float d_pfa; // probability of false alarm

    tm_type d_threshold_method;

    void _set_mark_delay(unsigned int mark_delay);
    void _set_threshold(float threshold);

public:
    corr_est_cc_impl(const std::vector<gr_complex>& symbols,
                     float sps,
                     unsigned int mark_delay,
                     float threshold = 0.9,
                     tm_type threshold_method = THRESHOLD_ABSOLUTE);
    ~corr_est_cc_impl();

    std::vector<gr_complex> symbols() const;
    void set_symbols(const std::vector<gr_complex>& symbols);

    unsigned int mark_delay() const;
    void set_mark_delay(unsigned int mark_delay);

    float threshold() const;
    void set_threshold(float threshold);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_CORR_EST_CC_IMPL_H */
