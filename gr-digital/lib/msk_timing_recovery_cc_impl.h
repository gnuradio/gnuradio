/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_MSK_TIMING_RECOVERY_CC_IMPL_H
#define INCLUDED_DIGITAL_MSK_TIMING_RECOVERY_CC_IMPL_H

#include <gnuradio/digital/msk_timing_recovery_cc.h>
#include <gnuradio/filter/fir_filter_with_buffer.h>
#include <gnuradio/filter/mmse_fir_interpolator_cc.h>
#include <boost/circular_buffer.hpp>

namespace gr {
namespace digital {

class msk_timing_recovery_cc_impl : public msk_timing_recovery_cc
{
private:
    float d_sps;
    float d_gain;
    float d_limit;
    filter::mmse_fir_interpolator_cc* d_interp;
    gr_complex d_dly_conj_1, d_dly_conj_2, d_dly_diff_1;
    float d_mu, d_omega, d_gain_omega;
    int d_div;
    int d_osps;

public:
    msk_timing_recovery_cc_impl(float sps, float gain, float limit, int osps);
    ~msk_timing_recovery_cc_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
    void set_gain(float gain);
    float get_gain(void);

    void set_limit(float limit);
    float get_limit(void);

    void set_sps(float sps);
    float get_sps(void);
};
} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_MSK_TIMING_RECOVERY_CC_IMPL_H */
